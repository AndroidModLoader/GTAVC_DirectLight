#include <mod/amlmod.h>
#include <mod/logger.h>

uintptr_t pGTAVC;
void *hGTAVC;

MYMOD(net.rusjj.directlight, DirectLight, 1.0, DK22pac & xanser & RusJJ)
NEEDGAME(com.rockstargames.gtavc)

void (*ActivateDirectional)();
void (*SetAmbientColoursForPedsCarsAndObjects)();
void (*WorldReplaceNormalLightsWithScorched)(uintptr_t sceneUnused, float);
void (*SetAmbientColours)();
void (*DeActivateDirectional)();

DECL_HOOKb(EntitySetupLighting, uintptr_t entity)
{
    ActivateDirectional();
    SetAmbientColoursForPedsCarsAndObjects();

    return true;
}

DECL_HOOKb(ObjectSetupLighting, uintptr_t entity)
{
    ActivateDirectional();
    SetAmbientColoursForPedsCarsAndObjects();

    uint16_t modelIndex = *(uint16_t*)(entity + BYBIT(0x60, 0x70));
    if(modelIndex >= 240 && modelIndex <= 245)
    {
        if((*(char*)(entity + BYBIT(0x56, 0x66)) & 0x10) != 0) // flag.scorched
        {
            WorldReplaceNormalLightsWithScorched(0, 0.1f);
        }
    }

    return true;
}

DECL_HOOKv(EntityRemoveLighting, uintptr_t entity, bool reset)
{
    SetAmbientColours();
    DeActivateDirectional();
}

extern "C" void OnModLoad()
{
    logger->SetTag("Direct Light");
    pGTAVC = aml->GetLib("libGTAVC.so");
    hGTAVC = aml->GetLibHandle("libGTAVC.so");
    
    if(!pGTAVC || !hGTAVC)
    {
        logger->Error("This mod is obviously works in Vice City only. Aborting.");
        return;
    }

    logger->Info("Warming up the code...");
    
    HOOK(EntitySetupLighting, aml->GetSym(hGTAVC, "_ZN7CEntity13SetupLightingEv"));
    HOOK(ObjectSetupLighting, aml->GetSym(hGTAVC, "_ZN7CObject13SetupLightingEv"));
    HOOK(EntityRemoveLighting, aml->GetSym(hGTAVC, "_ZN7CEntity14RemoveLightingEb"));
    HOOK(EntityRemoveLighting, aml->GetSym(hGTAVC, "_ZN7CObject14RemoveLightingEb"));

    SET_TO(ActivateDirectional, aml->GetSym(hGTAVC, "_Z19ActivateDirectionalv"));
    SET_TO(SetAmbientColoursForPedsCarsAndObjects, aml->GetSym(hGTAVC, "_Z38SetAmbientColoursForPedsCarsAndObjectsv"));
    SET_TO(WorldReplaceNormalLightsWithScorched, aml->GetSym(hGTAVC, "_Z36WorldReplaceNormalLightsWithScorchedP7RpWorldf"));
    SET_TO(SetAmbientColours, aml->GetSym(hGTAVC, "_Z17SetAmbientColoursv"));
    SET_TO(DeActivateDirectional, aml->GetSym(hGTAVC, "_Z21DeActivateDirectionalv"));
}