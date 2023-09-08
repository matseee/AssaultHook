#pragma once
#include "hack.h"
#include "../memory/memory.h"

template <typename T>
class Freeze : public Hack {
public:
    inline Freeze(addr address, T value) : Hack() {
        m_Address = address;
        m_Value = value;
    };

    inline ~Freeze() {
        Deactivate();
        m_Address = NULL;
        m_Value = NULL;
	    m_AcState = nullptr;
    }

    inline void Tick() {
		if (!IsActive()) {
			return;
		}

        T* typedValue = reinterpret_cast<T*>(m_Address);
		*typedValue = m_Value;
    };

protected:
    addr m_Address;
    T m_Value;
};

class Patch : public Hack {
public:
    Patch(addr address, addr valueOn, addr valueOff, ulong size);
    ~Patch();
    void Activate();
    void Deactivate();
protected:
    addr m_Address;
    addr m_ValueOn;
    addr m_ValueOff;
    ulong m_Size;

    void MemPatch();
};
