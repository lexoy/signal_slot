/*******************************************************************
 *  Copyleft(c) 2015 under LGPL license
 *   
 *  FileName         : signal.h
 *  BriefDescription : Implements signal-slot functions.
 *  
 *  CreateDate  : 2016 / 7 / 21
 *  Author      : lexo
 *  Mail        : lexo_y@icloud.com
 *  Description : New create this head file.
 *  
 *  ModifyDate  : N / A
 *  Author      : N / A
 *  Mail        : N / A
 *  Description : N / A
 ******************************************************************/

#include "def.h"

/* Implements of class entity<Return_Type (*)(Tparams...)> */
template <typename Return_Type, typename... Tparams>
entity_base<Return_Type, Tparams...>::~entity_base(void)
{
}

template <typename Return_Type, typename... Tparams>
entity<Return_Type (*)(Tparams...)>::entity(function function_)
{
	this->function_ = function_;
}

template <typename Return_Type, typename... Tparams>
entity<Return_Type (*)(Tparams...)>::entity(void)
{
	this->function_ = nullptr;
}

template <typename Return_Type, typename... Tparams>
entity<Return_Type (*)(Tparams...)>::~entity(void)
{
	this->function_ = nullptr;
}

template <typename Return_Type, typename... Tparams>
inline void
entity<Return_Type (*)(Tparams...)>::register_entity(function function_)
{
	this->function_ = function_;
}

template <typename Return_Type, typename... Tparams>
inline Slot*
entity<Return_Type (*)(Tparams...)>::getdest(void) const
{
	return reinterpret_cast<Slot*>(this->function_);
}

template <typename Return_Type, typename... Tparams>
inline void*
entity<Return_Type (*)(Tparams...)>::getmemfunPtr(void) const
{
	return nullptr;
}

template <typename Return_Type, typename... Tparams>
Return_Type
entity<Return_Type (*)(Tparams...)>::emitSignal(Tparams const& ... params)
{
	return this->function_(params...);
}


/* Implements of class entity<Return_Type (Class_Type::*)(Tparams...)> */
template <typename Class_Type, typename Return_Type, typename... Tparams>
entity<Return_Type (Class_Type::*)(Tparams...)>::entity(Class_Type* class_, function function_)
{
	this->class_    = class_;
	this->function_ = function_;
}

template <typename Class_Type, typename Return_Type, typename... Tparams>
entity<Return_Type (Class_Type::*)(Tparams...)>::entity(void)
{
	this->class_    = nullptr;
	this->function_ = nullptr;
}

template <typename Class_Type, typename Return_Type, typename... Tparams>
entity<Return_Type (Class_Type::*)(Tparams...)>::~entity(void)
{
	this->class_    = nullptr;
	this->function_ = nullptr;
}

template <typename Class_Type, typename Return_Type, typename... Tparams>
inline void
entity<Return_Type (Class_Type::*)(Tparams...)>::register_entity(Class_Type* class_, function function_)
{
	this->class_    = class_;
	this->function_ = function_;
}

template <typename Class_Type, typename Return_Type, typename... Tparams>
inline Slot*
entity<Return_Type (Class_Type::*)(Tparams...)>::getdest(void) const
{
	return this->class_;
}

template <typename Class_Type, typename Return_Type, typename... Tparams>
inline void*
entity<Return_Type (Class_Type::*)(Tparams...)>::getmemfunPtr(void) const
{
	return reinterpret_cast<void*>(this->function_);
}

template <typename Class_Type, typename Return_Type, typename... Tparams>
Return_Type
entity<Return_Type (Class_Type::*)(Tparams...)>::emitSignal(Tparams const& ... params)
{
	return (this->class_->*function_)(params...);
}


/* Implements of class Signal_base */
template <typename Return_Type, typename... Tparams>
Signal_base<Return_Type, Tparams...>::~Signal_base(void)
{
	disconnect_all();
}

template <typename Return_Type, typename... Tparams>
void
Signal_base<Return_Type, Tparams...>::disconnect_all(void)
{
	typename entity_list::iterator it    = m_connected_slots.begin();
	typename entity_list::iterator itEnd = m_connected_slots.end();

	while (it != itEnd)
	{
		if ((*it)->getdest())
			(*it)->getdest()->signal_disconnect(this);
		delete *it;
		++it;
	}
	m_connected_slots.erase(m_connected_slots.begin(), m_connected_slots.end());

	it    = Signal_base<Return_Type, Tparams...>::m_connected_funptr.begin();
	itEnd = Signal_base<Return_Type, Tparams...>::m_connected_funptr.end();

	while (it != itEnd)
	{
		delete *it;
		it = m_connected_funptr.erase(it);
	}
}

template <typename Return_Type, typename... Tparams>
void
Signal_base<Return_Type, Tparams...>::disconnect(Slot* pclass)
{
	typename entity_list::iterator it    = m_connected_slots.begin();
	typename entity_list::iterator itEnd = m_connected_slots.end();

	while (it != itEnd)
	{
		if ((*it)->getdest() == pclass)
		{
			delete *it;
			it = m_connected_slots.erase(it);
			pclass->signal_disconnect(this);
		} else
			++it;
	}
}

template <typename Return_Type, typename... Tparams>
template <typename R, typename... T>
void
Signal_base<Return_Type, Tparams...>::disconnect(R (*pfun)(T...))
{
    typename entity_list::iterator it    = m_connected_funptr.begin();
	typename entity_list::iterator itEnd = m_connected_funptr.end();

	while (it != itEnd)
	{
		if (reinterpret_cast<Slot*>(pfun) == (*it)->getdest())
		{
			m_connected_funptr.erase(it);
			return;
		}
		++it;
	}
}

template <typename Return_Type, typename... Tparams>
void
Signal_base<Return_Type, Tparams...>::slot_disconnect(Slot* pslot)
{
	typename entity_list::iterator it    = m_connected_slots.begin();
	typename entity_list::iterator itEnd = m_connected_slots.end();

	while (it != itEnd)
	{
		if ((*it)->getdest() == pslot)
		{
			delete *it;
			it = m_connected_slots.erase(it);
		} else
			++it;
	}
}


/* Implements of class Slot */
Slot::~Slot()
{
	disconnect_all();
}

void
Slot::disconnect_all()
{
	const_iterator it    = m_signal.begin();
	const_iterator itEnd = m_signal.end();

	while (it != itEnd)
	{
		(*it)->slot_disconnect(this);
		++it;
	}
	m_signal.erase(m_signal.begin(), m_signal.end());
}

void
Slot::signal_connect(_Signal_base* signal)
{
	m_signal.insert(signal);
}

void
Slot::signal_disconnect(_Signal_base* signal)
{
	m_signal.erase(signal);
}


/* Implements of class Signal */
template <typename Return_Type, typename... Tparams>
template <typename Class_Type>
connect_result
Signal<Return_Type, Tparams...>::connect(Class_Type* pclass, Return_Type (Class_Type::*pmemfun)(Tparams...))
{
	entity<Return_Type (Class_Type::*)(Tparams...)>* memfun_entity = new entity<Return_Type (Class_Type::*)(Tparams...)>(pclass, pmemfun);
	Signal_base<Return_Type, Tparams...>::m_connected_slots.push_back(memfun_entity);
	pclass->signal_connect(this);
	return SUCCESS;
}

template <typename Return_Type, typename... Tparams>
connect_result
Signal<Return_Type, Tparams...>::connect(Return_Type (*pfun)(Tparams...))
{
	entity<Return_Type (*)(Tparams...)>* function_entity = new entity<Return_Type (*)(Tparams...)>(pfun);
	Signal_base<Return_Type, Tparams...>::m_connected_funptr.push_back(function_entity);
	return SUCCESS;
}

template <typename Return_Type, typename... Tparams>
typename Signal<Return_Type, Tparams...>::Rv_Vector_Set_Sp
Signal<Return_Type, Tparams...>::operator()(Tparams... params)
{
	typename std::list<entity_base<Return_Type, Tparams...>*>::const_iterator it
	                                 = Signal_base<Return_Type, Tparams...>::m_connected_slots.begin();
	typename std::list<entity_base<Return_Type, Tparams...>*>::const_iterator itEnd
                                     = Signal_base<Return_Type, Tparams...>::m_connected_slots.end();

    std::vector<std::shared_ptr<rv_set<Return_Type>>> ret_vals;
	while (it != itEnd)
	{
		std::shared_ptr<rv_set<Return_Type>> sp_rvset(new rv_set<Return_Type>);
		sp_rvset->function_ = (*it)->getmemfunPtr();
		sp_rvset->class_ = (*it)->getdest();
		sp_rvset->value = (*it)->emitSignal(params...);
		ret_vals.push_back(sp_rvset);
		++it;
	}

	it    = Signal_base<Return_Type, Tparams...>::m_connected_funptr.begin();
	itEnd = Signal_base<Return_Type, Tparams...>::m_connected_funptr.end();

	while (it != itEnd)
	{
		std::shared_ptr<rv_set<Return_Type>> sp_rvset(new rv_set<Return_Type>);
		sp_rvset->function_ = (*it)->getdest();
		sp_rvset->class_ = nullptr;
		sp_rvset->value = (*it)->emitSignal(params...);
		ret_vals.push_back(sp_rvset);
		++it;
	}

	return ret_vals;
}