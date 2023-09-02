#include "acState.h"

AcState* AcState::Instance;

AcState* AcState::Get() {
    if (!AcState::Instance) {
        Log::Debug() << "AcState::Get(): No \"AcState::Instance\" found. Creating one ..." << Log::Endl;
        AcState::Instance = new AcState();
        Log::Debug() << "AcState::Get(): Creation done ..." << Log::Endl;
    }
	return AcState::Instance;
}

void AcState::Destroy() {
    delete AcState::Instance;
    Log::Debug() << "AcState::Destroy(): \"AcState::Instance\" destroyed ..." << Log::Endl;
}

AcState::AcState() {
    this->Setup();
}

AcState::~AcState() {
    this->Matrix = nullptr;
    this->GameMode = nullptr;
    this->LocalPlayer = nullptr;
    this->EntityList = nullptr;
    this->PlayerCount = nullptr;

    this->ModuleBase = NULL;
}

bool AcState::IsReady() {
    if (!this->CheckReady()) {
        this->Setup();
    }
    return this->CheckReady();
}

void AcState::Setup() {
    this->LoadModules();
    this->UpdateAttributes();
    Log::Debug() << "AcState::Setup(): Done ..." << Log::Endl;
}

bool AcState::CheckReady() {
    return this->ModuleBase;
}

bool AcState::IsTeamGame() {
	int* gameMode = this->GameMode;
	return *gameMode == 0 || *gameMode == 4 
		|| *gameMode == 5 || *gameMode == 7 
		|| *gameMode == 11 || *gameMode == 13 
		|| *gameMode == 14 || *gameMode == 16 
		|| *gameMode == 17 || *gameMode == 20 
		|| *gameMode == 21;
}

bool AcState::IsEnemy(AcEntity* entity) {
	return !this->IsTeamGame() || entity->Team != this->LocalPlayer->Team;
}

bool AcState::IsValidEntity(AcEntity* entity) {
	return entity && entity->VTable == ADDR_ENTITY_VTABLE_TO_COMPARE
		&& entity->Health > 0;
}
void AcState::LoadModules() {
    this->ModuleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    if (!this->ModuleBase) {
        Log::Error() << "AcState::LoadModules(): Could not get \"ac_client.exe\" module handle ..." << Log::Endl;
    }
    Log::Debug() << "AcState::LoadModules(): GetModuleHandle(\"ac_client.exe\") => " << (void*)this->ModuleBase << Log::Endl;
}

void AcState::UpdateAttributes() {
    if (!this->CheckReady()) {
        Log::Warning() << "AcState::UpdateAttributes(): Not ready! Returned without updating attributes ..." << Log::Endl;
        return;
    }
    this->Matrix = (float*)(ADDR_MATRIX);
    this->GameMode = (int*)(this->ModuleBase + ADDR_GAME_MODE);
    this->LocalPlayer = *((AcEntity**)(this->ModuleBase + ADDR_ENTITY_LOCALPLAYER));
    this->EntityList = *((AcEntityList**)(this->ModuleBase + ADDR_ENTITY_LIST));
    this->PlayerCount = (int*)(this->ModuleBase + ADDR_NUM_PLAYERS);

    Log::Debug() << "AcState::UpdateAttributes(): Done ..." << Log::Endl;
}
