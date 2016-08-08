/*******************************************************************
 *  Copyleft(c) 2015 under LGPL license
 *   
 *  FileName         : def.h
 *  BriefDescription : Common define to signal.h.
 *  
 *  CreateDate  : 2016 / 7 / 3
 *  Author      : lexo
 *  Mail        : lexo_y@icloud.com
 *  Description : New create this head file.
 *  
 *  ModifyDate  : N / A
 *  Author      : N / A
 *  Mail        : N / A
 *  Description : N / A
 ******************************************************************/

#pragma once

#include <vector>
#include <memory>   // For shared_ptr
#include <string>
#include <list>
#include <set>

class Slot;

enum connect_result
{
	SUCCESS   = 0 ,
	FAIL      = 10,
	EXCEPTION = 20
};

template <typename Value_Type>
struct rv_set
{
	Value_Type  value    ;
	void*       class_   ;
	void*       function_;
};

template <typename Return_Type, typename... Tparams>
class entity_base
{
public:
	virtual Slot*       getdest     (void) const          = 0;
	virtual void*       getmemfunPtr(void) const          = 0;
	virtual Return_Type emitSignal  (Tparams const& ...)  = 0;
	virtual             ~entity_base(void)                   ;
};

template <typename F>
class entity;

template <typename Return_Type, typename... Tparams>
class entity<Return_Type (*)(Tparams...)> : public entity_base<Return_Type, Tparams...>
{
protected:
	using function = Return_Type (*)(Tparams...);

public:
	entity (function);
	entity (void)    ;
	~entity(void)    ;

public:
	void        register_entity(function)          ;
	Slot*       getdest        (void) const        ;
	void*       getmemfunPtr   (void) const        ;
	Return_Type emitSignal     (Tparams const& ...);

private:
	function function_;
};

template <typename Class_Type, typename Return_Type, typename... Tparams>
class entity<Return_Type (Class_Type::*)(Tparams...)> : public entity_base<Return_Type, Tparams...>
{
protected:
	using function = Return_Type (Class_Type::*)(Tparams...);

public:
	entity (Class_Type*, function);
	entity (void)                 ;
	~entity(void)                 ;

public:
	void        register_entity(Class_Type*, function);
	Slot*       getdest        (void) const           ;
	void*       getmemfunPtr   (void) const           ;
	Return_Type emitSignal     (Tparams const& ...)   ;

private:
	Class_Type* class_;
	function function_;
};


class _Signal_base
{
public:
	virtual void slot_disconnect(Slot*) = 0;
};

template <typename Return_Type, typename... Tparams>
class Signal_base : public _Signal_base
{
public:
	using entity_list = std::list<entity_base<Return_Type, Tparams...>*>;

	~Signal_base(void);

	void slot_disconnect(Slot*);
	void disconnect_all (void) ;
	void disconnect     (Slot*);

	template <typename R, typename... T>
	void disconnect(R (*)(T...));

protected:
	entity_list m_connected_slots;
	entity_list m_connected_funptr;
};


class Slot
{
private:
	using signal_set     = std::set<_Signal_base*>;
	using const_iterator = signal_set::const_iterator;

public:
	~Slot();

	void disconnect_all   ()             ;
	void signal_connect   (_Signal_base*);
	void signal_disconnect(_Signal_base*);

private:
	signal_set m_signal;
};


template <typename Return_Type, typename... Tparams>
class Signal : public Signal_base<Return_Type, Tparams...>
{
public:
	using Rv_Vector_Set_Sp = std::vector<std::shared_ptr<rv_set<Return_Type>>>;

	Rv_Vector_Set_Sp operator()(Tparams... params);

	template <typename Class_Type>
	connect_result connect(Class_Type*, Return_Type (Class_Type::*pmemfun)(Tparams...));
	connect_result connect(Return_Type (*pfun)(Tparams...));
};