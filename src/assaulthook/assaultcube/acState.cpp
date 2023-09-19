#include "acState.h"

#ifdef _LINUX
const char* MODULE_NAME = "ac_client";
#endif

#ifdef _WINDOWS
const char* MODULE_NAME = "ac_client.exe";
#endif

AcState* AcState::Instance;

AcState* AcState::Get() {
    if (!AcState::Instance) {
        Log::Debug() << "AcState::Get(): No \"AcState::Instance\" found. Creating one ..." << Log::Endl;
        AcState::Instance = new AcState();
        Log::Debug() << "AcState::Get(): Creation done ..." << Log::Endl;
    }
    return AcState::Instance;
}

void AcState::Reload() {
    if (AcState::Instance) {
        AcState::Instance->ScanForSignatures();
    }
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

    ModuleBase = 0;

    NoRecoil = 0;
    DecreaseAmmo = 0;
    DecreaseHealth = 0;
    IntersectClosest = 0;
    IntersectGeometry = 0;
}

bool AcState::IsReady() {
    if (!CheckReady()) {
        Setup();
    }
    return CheckReady();
}

void AcState::Setup() {
    LoadModules();
    ScanForSignatures();
    Log::Debug() << "AcState::Setup(): Done ..." << Log::Endl;
}

bool AcState::CheckReady() {
    return ModuleBase;
}

bool AcState::IsTeamGame() {
    int* gameMode = GameMode;
    return *gameMode == 0 || *gameMode == 4 || *gameMode == 5 || *gameMode == 7
        || *gameMode == 11 || *gameMode == 13 || *gameMode == 14 || *gameMode == 16
        || *gameMode == 17 || *gameMode == 20 || *gameMode == 21;
}

bool AcState::IsEnemy(AcEntity* entity) {
    return !IsTeamGame() || entity->Team != LocalPlayer->Team;
}

bool AcState::IsValidEntity(AcEntity* entity) {
    return entity && entity->Health > 0;
}

void AcState::LoadModules() {
    ModuleBase = memory::GetModuleBaseAddress(MODULE_NAME);
    if (!ModuleBase) {
        Log::Error() << "AcState::LoadModules(): Could not get \"ac_client\" module handle ..." << Log::Endl;
    }
    Log::Debug() << "AcState::LoadModules(): GetModuleHandle(\"ac_client\") => 0x" << (void*)ModuleBase << Log::Endl;
}

bool AcState::ScanForSignatures() {
    uint sizeOfModule = memory::GetModuleSize(ModuleBase);
    if (!sizeOfModule) {
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

        // sigGameMode
        memory::Signature { "89 15 ? ? ? ? 53", 2 },
        // sigMatrix
        memory::Signature { "F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 28 ? 0F C6 C3 ? F3 0F ? ? ? ? ? ? F3 0F ? ? F3 0F ? ? F2 0F ? ? ? ? ? ? 0F 28 ? 0F 54 ? ? ? ? ? 0F 5A ? 66 0F ? ? 77 ? F3 0F", 4 },
        // sigLocalPlayer
        memory::Signature { "8B 0D ? ? ? ? 56 57 8B 3D", 2 },
        // sigEntityList
        memory::Signature { "A1 ? ? ? ? ? ? ? ? F6 0F 84 5F", 1 },
        // sigPlayerCount
        memory::Signature { "8B 0D ? ? ? ? 46 3B ? 7C ? 8B 35", 2 },
    };

    memory::SignatureScanner* scanner = new memory::SignatureScanner(ModuleBase, sizeOfModule);
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

    Log::Info() << "AcState::ScanForSignatures(): Found GameMode signature at 0x" << (void*)signatures[5].address << " ..." << Log::Endl;
    GameMode = (int*)(*(addr*)signatures[5].address);

    Log::Info() << "AcState::ScanForSignatures(): Found Matrix signature at 0x" << (void*)signatures[6].address << " ..." << Log::Endl;
    Matrix = (float*)(*(addr*)signatures[6].address);

    Log::Info() << "AcState::ScanForSignatures(): Found LocalPlayer signature at 0x" << (void*)signatures[7].address << " ..." << Log::Endl;
    LocalPlayer = *(AcEntity**)(*(addr*)signatures[7].address);

    Log::Info() << "AcState::ScanForSignatures(): Found EntityList signature at 0x" << (void*)signatures[8].address << " ..." << Log::Endl;
    EntityList = *(AcEntityList**)(*(addr*)signatures[8].address);

    Log::Info() << "AcState::ScanForSignatures(): Found PlayerCount signature at 0x" << (void*)signatures[9].address << " ..." << Log::Endl;
    PlayerCount = (int*)(*(addr*)signatures[9].address);
    return true;
}

