/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/GameLogic/DEV_PythonMouse.cpp
 *  \ingroup gamelogic
 */


#include "DEV_PythonMouse.h"
#include "SCA_IInputDevice.h"
#include "RAS_ICanvas.h"
#include <cmath>

/* ------------------------------------------------------------------------- */
/* Native functions                                                          */
/* ------------------------------------------------------------------------- */

DEV_PythonMouse::DEV_PythonMouse(SCA_IInputDevice *mouse, RAS_ICanvas *canvas)
	:EXP_PyObjectPlus(),
	m_mouse(mouse),
	m_canvas(canvas)
{
#ifdef WITH_PYTHON
	m_event_dict = PyDict_New();
#endif
}

DEV_PythonMouse::~DEV_PythonMouse()
{
#ifdef WITH_PYTHON
	PyDict_Clear(m_event_dict);
	Py_DECREF(m_event_dict);
#endif
}

void DEV_PythonMouse::CalculateDelta()
{
	// mouse position
	const SCA_InputEvent& xevent = this->m_mouse->GetInput(SCA_IInputDevice::MOUSEX);
	const SCA_InputEvent& yevent = this->m_mouse->GetInput(SCA_IInputDevice::MOUSEY);

	float mouseX = this->m_canvas->GetMouseNormalizedX(xevent.m_values[xevent.m_values.size() - 1]);
	float mouseY = this->m_canvas->GetMouseNormalizedY(yevent.m_values[yevent.m_values.size() - 1]);

	float x = mouseX - this->m_oldPosition.x;
	float y = mouseY - this->m_oldPosition.y;

	if (std::round(x * this->m_canvas->GetWidth()) == 1) {
		x = 0.0;
	}
	if (std::round(y * this->m_canvas->GetHeight()) == 1) {
		y = 0.0;
	}

	this->m_deltaPosition.x = -x;
	this->m_deltaPosition.y = -y;

	this->m_oldPosition.x = mouseX;
	this->m_oldPosition.y = mouseY;

	// printf("x:%lf, y:%lf\n", this->m_deltaPosition.x, this->m_deltaPosition.y);
}

void DEV_PythonMouse::Centralize()
{
	double widthHalf = this->m_canvas->GetWidth() * 0.5;
	double heightHalf = this->m_canvas->GetHeight() * 0.5;

	this->m_oldPosition.x = widthHalf / this->m_canvas->GetWidth();
	this->m_oldPosition.y = heightHalf / this->m_canvas->GetHeight();

	this->m_canvas->SetMousePosition((int)(widthHalf), (int)(heightHalf));
}

#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */
PyTypeObject DEV_PythonMouse::Type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"DEV_PythonMouse",
	sizeof(EXP_PyObjectPlus_Proxy),
	0,
	py_base_dealloc,
	0,
	0,
	0,
	0,
	py_base_repr,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	0, 0, 0, 0, 0, 0, 0,
	Methods,
	0,
	0,
	&EXP_PyObjectPlus::Type,
	0, 0, 0, 0, 0, 0,
	py_base_new
};

PyMethodDef DEV_PythonMouse::Methods[] = {
	{"centralize", (PyCFunction)DEV_PythonMouse::sPyCentralizeCursor, METH_NOARGS},
	{nullptr, nullptr} //Sentinel
};

PyAttributeDef DEV_PythonMouse::Attributes[] = {
	EXP_PYATTRIBUTE_RO_FUNCTION("events", DEV_PythonMouse, pyattr_get_events),
	EXP_PYATTRIBUTE_RO_FUNCTION("inputs", DEV_PythonMouse, pyattr_get_inputs),
	EXP_PYATTRIBUTE_RO_FUNCTION("active_events", DEV_PythonMouse, pyattr_get_active_events),
	EXP_PYATTRIBUTE_RO_FUNCTION("activeInputs", DEV_PythonMouse, pyattr_get_active_inputs),
	EXP_PYATTRIBUTE_RO_FUNCTION("deltaPosition", DEV_PythonMouse, pyattr_get_deltaPosition),
	EXP_PYATTRIBUTE_RW_FUNCTION("position", DEV_PythonMouse, pyattr_get_position, pyattr_set_position),
	EXP_PYATTRIBUTE_RW_FUNCTION("visible", DEV_PythonMouse, pyattr_get_visible, pyattr_set_visible),
	EXP_PYATTRIBUTE_NULL    //Sentinel
};

PyObject *DEV_PythonMouse::PyCentralizeCursor()
{
	Centralize();
	Py_RETURN_NONE;
}

PyObject *DEV_PythonMouse::pyattr_get_events(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	EXP_ShowDeprecationWarning("mouse.events", "mouse.inputs");

	for (int i = SCA_IInputDevice::BEGINMOUSE; i <= SCA_IInputDevice::ENDMOUSE; i++)
	{
		SCA_InputEvent& input = self->m_mouse->GetInput((SCA_IInputDevice::SCA_EnumInputs)i);
		int event = 0;
		if (input.m_queue.empty()) {
			event = input.m_status[input.m_status.size() - 1];
		}
		else {
			event = input.m_queue[input.m_queue.size() - 1];
		}

		PyObject *key = PyLong_FromLong(i);
		PyObject *value = PyLong_FromLong(event);

		PyDict_SetItem(self->m_event_dict, key, value);

		Py_DECREF(key);
		Py_DECREF(value);
	}
	Py_INCREF(self->m_event_dict);
	return self->m_event_dict;
}

PyObject *DEV_PythonMouse::pyattr_get_inputs(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	for (int i = SCA_IInputDevice::BEGINMOUSE; i <= SCA_IInputDevice::ENDMOUSE; i++)
	{
		SCA_InputEvent& input = self->m_mouse->GetInput((SCA_IInputDevice::SCA_EnumInputs)i);

		PyObject *key = PyLong_FromLong(i);

		PyDict_SetItem(self->m_event_dict, key, input.GetProxy());

		Py_DECREF(key);
	}
	Py_INCREF(self->m_event_dict);
	return self->m_event_dict;
}

PyObject *DEV_PythonMouse::pyattr_get_active_events(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	EXP_ShowDeprecationWarning("mouse.active_events", "mouse.activeInputs");

	PyDict_Clear(self->m_event_dict);

	for (int i = SCA_IInputDevice::BEGINMOUSE; i <= SCA_IInputDevice::ENDMOUSE; i++)
	{
		SCA_InputEvent& input = self->m_mouse->GetInput((SCA_IInputDevice::SCA_EnumInputs)i);

		if (input.Find(SCA_InputEvent::ACTIVE)) {
			int event = 0;
			if (input.m_queue.empty()) {
				event = input.m_status[input.m_status.size() - 1];
			}
			else {
				event = input.m_queue[input.m_queue.size() - 1];
			}

			PyObject *key = PyLong_FromLong(i);
			PyObject *value = PyLong_FromLong(event);

			PyDict_SetItem(self->m_event_dict, key, value);

			Py_DECREF(key);
			Py_DECREF(value);
		}
	}
	Py_INCREF(self->m_event_dict);
	return self->m_event_dict;
}

PyObject *DEV_PythonMouse::pyattr_get_active_inputs(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	PyDict_Clear(self->m_event_dict);

	for (int i = SCA_IInputDevice::BEGINMOUSE; i <= SCA_IInputDevice::ENDMOUSE; i++)
	{
		SCA_InputEvent& input = self->m_mouse->GetInput((SCA_IInputDevice::SCA_EnumInputs)i);

		if (input.Find(SCA_InputEvent::ACTIVE)) {
			PyObject *key = PyLong_FromLong(i);

			PyDict_SetItem(self->m_event_dict, key, input.GetProxy());

			Py_DECREF(key);
		}
	}
	Py_INCREF(self->m_event_dict);
	return self->m_event_dict;
}

PyObject *DEV_PythonMouse::pyattr_get_position(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);
	const SCA_InputEvent& xevent = self->m_mouse->GetInput(SCA_IInputDevice::MOUSEX);
	const SCA_InputEvent& yevent = self->m_mouse->GetInput(SCA_IInputDevice::MOUSEY);

	float x_coord, y_coord;

	x_coord = self->m_canvas->GetMouseNormalizedX(xevent.m_values[xevent.m_values.size() - 1]);
	y_coord = self->m_canvas->GetMouseNormalizedY(yevent.m_values[yevent.m_values.size() - 1]);

	PyObject *ret = PyTuple_New(2);

	PyTuple_SET_ITEM(ret, 0, PyFloat_FromDouble(x_coord));
	PyTuple_SET_ITEM(ret, 1, PyFloat_FromDouble(y_coord));

	return ret;
}

PyObject *DEV_PythonMouse::pyattr_get_deltaPosition(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	mathfu::vec2 delta = self->GetDeltaPosition();

	PyObject *ret = PyTuple_New(2);
	PyTuple_SET_ITEM(ret, 0, PyFloat_FromDouble(delta.x));
	PyTuple_SET_ITEM(ret, 1, PyFloat_FromDouble(delta.y));

	return ret;
}

int DEV_PythonMouse::pyattr_set_position(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);
	int x, y;
	float pyx, pyy;
	if (!PyArg_ParseTuple(value, "ff:position", &pyx, &pyy)) {
		return PY_SET_ATTR_FAIL;
	}

	x = (int)(pyx * self->m_canvas->GetMaxX());
	y = (int)(pyy * self->m_canvas->GetMaxY());

	self->m_canvas->SetMousePosition(x, y);

	return PY_SET_ATTR_SUCCESS;
}

PyObject *DEV_PythonMouse::pyattr_get_visible(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	int visible;

	if (self->m_canvas->GetMouseState() == RAS_ICanvas::MOUSE_INVISIBLE) {
		visible = 0;
	}
	else {
		visible = 1;
	}

	return PyBool_FromLong(visible);
}

int DEV_PythonMouse::pyattr_set_visible(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	DEV_PythonMouse *self = static_cast<DEV_PythonMouse *>(self_v);

	int visible = PyObject_IsTrue(value);

	if (visible == -1) {
		PyErr_SetString(PyExc_AttributeError, "DEV_PythonMouse.visible = bool: DEV_PythonMouse, expected True or False");
		return PY_SET_ATTR_FAIL;
	}

	if (visible) {
		self->m_canvas->SetMouseState(RAS_ICanvas::MOUSE_NORMAL);
	}
	else {
		self->m_canvas->SetMouseState(RAS_ICanvas::MOUSE_INVISIBLE);
	}

	return PY_SET_ATTR_SUCCESS;
}

#endif
