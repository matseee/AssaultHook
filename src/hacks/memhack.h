#pragma once
#include "hack.h"
#include "../memory/memory.h"

template <typename T>
class Freeze : public Hack {
public:
    inline Freeze(uintptr_t address, T value) : Hack() {
        this->m_Address = address;
        this->m_Value = value;
    };

    inline ~Freeze() {
        this->Deactivate();
        this->m_Address = NULL;
        this->m_Value = NULL;
	    this->m_AcState = nullptr;
    }

    inline void Tick() {
		if (!this->IsActive()) {
			return;
		}

        T* typedValue = reinterpret_cast<T*>(m_Address);
		*typedValue = this->m_Value;
    };

protected:
    uintptr_t m_Address;
    T m_Value;
};

class Patch : public Hack {
public:
    Patch(uintptr_t address, uintptr_t valueOn, uintptr_t valueOff, unsigned long size);
    ~Patch();
    void Activate();
    void Deactivate();
protected:
    uintptr_t m_Address;
    uintptr_t m_ValueOn;
    uintptr_t m_ValueOff;
    unsigned long m_Size;

    void MemPatch();
};
