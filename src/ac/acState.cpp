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
    Setup();
}

AcState::~AcState() {
    Matrix = nullptr;
    GameMode = nullptr;
    LocalPlayer = nullptr;
    EntityList = nullptr;
    PlayerCount = nullptr;

    ModuleBase = NULL;

	NoRecoil = NULL;
	DecreaseAmmo = NULL;
	DecreaseHealth = NULL;
	IntersectClosest = NULL;
	IntersectGeometry = NULL;
}

bool AcState::IsReady() {
    if (!CheckReady()) {
        Setup();
    }
    return CheckReady();
}

void AcState::Setup() {
    LoadModules();
    UpdateAttributes();
    ScanForSignatures();
    Log::Debug() << "AcState::Setup(): Done ..." << Log::Endl;
}

bool AcState::CheckReady() {
    return ModuleBase;
}

bool AcState::IsTeamGame() {
	int* gameMode = GameMode;
	return *gameMode == 0 || *gameMode == 4 
		|| *gameMode == 5 || *gameMode == 7 
		|| *gameMode == 11 || *gameMode == 13 
		|| *gameMode == 14 || *gameMode == 16 
		|| *gameMode == 17 || *gameMode == 20 
		|| *gameMode == 21;
}

bool AcState::IsEnemy(AcEntity* entity) {
	return !IsTeamGame() || entity->Team != LocalPlayer->Team;
}

bool AcState::IsValidEntity(AcEntity* entity) {
	return entity && entity->VTable == ADDR_ENTITY_VTABLE_TO_COMPARE
		&& entity->Health > 0;
}

void AcState::LoadModules() {
    ModuleBase = (addr)GetModuleHandle(L"ac_client.exe");
    if (!ModuleBase) {
        Log::Error() << "AcState::LoadModules(): Could not get \"ac_client.exe\" module handle ..." << Log::Endl;
    }
    Log::Debug() << "AcState::LoadModules(): GetModuleHandle(\"ac_client.exe\") => 0x" << (void*)ModuleBase << Log::Endl;
}

bool AcState::ScanForSignatures() {
    MODULEINFO moduleInfo = {};
    GetModuleInformation(GetCurrentProcess(), (HMODULE)ModuleBase, &moduleInfo, sizeof(moduleInfo));
    if (!moduleInfo.SizeOfImage) {
        Log::Error() << "AcState::ScanForSignatures(): Could not evaluate module size ..." << Log::Endl;
        return false;
    }

    memory::Signature signatures[] = {
        // sigNoRecoil
		memory::Signature { "83 EC ? 53 55 8B 6C ? ? 56 57 8B F9", },
		// sigDecreaseAmmo
		memory::Signature { "FF 08 8D 44", },
        // sigDecreaseHealth
		memory::Signature { "2B F1 29 73", 2 },
        // sigIntersectClosest
		memory::Signature { "83 EC ? A1 ? ? ? ? ? ? ? ? 24", },
        // sigIntersectGeometry
		memory::Signature { "55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 8B DA 8B D1", }, 
    };

	memory::SignatureScanner* scanner = new memory::SignatureScanner(ModuleBase, moduleInfo.SizeOfImage);
    if (!scanner->ScanMulti(signatures, (sizeof(signatures) / sizeof(memory::Signature)))) {
        Log::Error() << "AcState::ScanForSignatures() : Could not find all signatures ..." << Log::Endl;
        return false;
    }

    Log::Info() << "AcState::ScanForSignatures(): Found NoRecoil signature at 0x" << (void*)signatures[0].address << " ..." << Log::Endl;
    NoRecoil = signatures[0].address;

    Log::Info() << "AcState::ScanForSignatures(): Found DecreaseAmmo signature at 0x" << (void*)signatures[1].address << " ..." << Log::Endl;
    DecreaseAmmo = signatures[1].address;

    Log::Info() << "AcState::ScanForSignatures(): Found DecreaseHealth signature at 0x" << (void*)signatures[2].address << " ..." << Log::Endl;
    DecreaseHealth = signatures[2].address;
    
    Log::Info() << "AcState::ScanForSignatures(): Found IntersectClosest signature at 0x" << (void*)signatures[3].address << " ..." << Log::Endl;
    IntersectClosest = signatures[3].address;
    
    Log::Info() << "AcState::ScanForSignatures(): Found IntersectGeometry signature at 0x" << (void*)signatures[4].address << " ..." << Log::Endl;
    IntersectGeometry = signatures[4].address;
    return true;
}


void AcState::UpdateAttributes() {
    if (!CheckReady()) {
        Log::Warning() << "AcState::UpdateAttributes(): Not ready! Returned without updating attributes ..." << Log::Endl;
        return;
    }

    Matrix = (float*)(ADDR_MATRIX);
    GameMode = (int*)(ModuleBase + ADDR_GAME_MODE);
    LocalPlayer = *((AcEntity**)(ModuleBase + ADDR_ENTITY_LOCALPLAYER));
    EntityList = *((AcEntityList**)(ModuleBase + ADDR_ENTITY_LIST));
    PlayerCount = (int*)(ModuleBase + ADDR_NUM_PLAYERS);
    Log::Debug() << "AcState::UpdateAttributes(): Done ..." << Log::Endl;
}
