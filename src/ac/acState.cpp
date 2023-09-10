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
		memory::Signature { "\x83\xEC\x28\x53\x55\x8B\x6C","xxxxxxx", },                                // sigNoRecoil
		memory::Signature { "\xFF\x08\x8D\x44\x24\x1C", "xxxxxx", },                                    // sigDecreaseAmmo
		memory::Signature { "\x29\x73\x04\x8B\xC6", "xxxxx", },                                         // sigDecreaseHealth
		memory::Signature { "\x83\xEC\x1C\xA1\x00\xAC\x58\x00", "xxxxxxxx", },                          // sigIntersectClosest
		memory::Signature { "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x34\x01\x00\x00\x53", "xxxxxxxxxxxxx", }, // sigIntersectGeometry
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
