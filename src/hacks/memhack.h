#pragma once
#include "hack.h"
#include "../memory/memory.h"

template <typename T>
class Freeze : public Hack {
public:
    inline Freeze(uintptr_t address, T value) : Hack() {
        this->address = address;
        this->value = value;
    };

    inline ~Freeze() {
        this->Deactivate();
        this->address = NULL;
        this->value = NULL;
	    this->acState = nullptr;
    }

    inline void Tick() {
		if (!this->IsActive()) {
			return;
		}

        T* typedValue = reinterpret_cast<T*>(address);
		*typedValue = this->value;
    };

protected:
    uintptr_t address;
    T value;
};

class Patch : public Hack {
public:
    Patch(uintptr_t address, uintptr_t valueOn, uintptr_t valueOff, unsigned long size);
    ~Patch();
    void Activate();
    void Deactivate();
protected:
    uintptr_t address;
    uintptr_t valueOn;
    uintptr_t valueOff;
    unsigned long size;

    void MemPatch();
};
