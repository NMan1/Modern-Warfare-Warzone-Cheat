#include "Render.h"
#include "Entity.h"
#include "ESP.h"
#include "Camera.h"
#include "Global.h"
#include "Utlilites.h"
#include "Settings.h"

#define MAX_PARTY 3

using GetDecryptedPos_t = void(__stdcall*)(uintptr_t, Vector*);
static GetDecryptedPos_t GetDecryptedPos_fn = nullptr;
bool bInplane = false;
cl_entity* pTeamArray[3];

void ESP::Draw()
{
    auto CG_Entitys = Entity::GetCGEntitys();
    if (!CG_Entitys)
        return;

    Globals::pRefDef = Globals::GetRefDef();
    if (!Globals::pRefDef)
        return;

    //if (CG_Entitys)
    //{
    //    // Plane Team check
    //    if (Globals::GetLocalPlayerPos().z < 0)
    //    {
    //        std::vector<cl_entity*> vEntity;
    //        for (int i = 0; i < 5; i++) // 200 to only get players
    //        {
    //            cl_entity* pEntity = (cl_entity*)(CG_Entitys + 0x318 * i);
    //            if (!pEntity)
    //                continue;

    //            // more than 3 therefore not in plane
    //            if (i > 3 && pEntity->type == 1)
    //            {
    //                vEntity.clear();
    //                break;
    //            }

    //            if (i < 3 && pEntity->type == 1)
    //                    vEntity.push_back(pEntity);
    //        }

    //        for (int i = 0; i < vEntity.size(); i++) // 600
    //            pTeamArray[i] = vEntity[i];
    //        
    //        Utils::Log("", vEntity[0]);
    //        Utils::Log("", vEntity[1]);
    //        Utils::Log("", vEntity[2]);

    //        vEntity.clear();
    //        bInplane = true;
    //    }
    //    else
    //        bInplane = false;

    //    if (bInplane)
    //        return;

        for (int i = 0; i < 800; i++) // 600
        {
            cl_entity* pEntity = (cl_entity*)(CG_Entitys + 0x318 * i);
            if (!pEntity)
                continue;       

            int type = pEntity->type;
            if ((type == 1 && pEntity->valid & 1) && (Settings::bSnapLinesToggle || Settings::bTextToggle)) // player
            {
                auto pFnc = (*(uint32_t*)(Globals::pGameBase + 0x53389FC) ^ ((uintptr_t)pEntity + 0x38) ^ *(uint64_t*)((uintptr_t)pEntity + 0x40));
                GetDecryptedPos_fn = (GetDecryptedPos_t)(pFnc);

                if (!pFnc || !GetDecryptedPos_fn)
                    continue;

                Vector validLocation;
                GetDecryptedPos_fn((uintptr_t)pEntity + 0x48, &validLocation);

                Vector vLocalPos = Globals::GetLocalPlayerPos();
                if ((vLocalPos - validLocation).x <= 1.f && (vLocalPos - validLocation).x >= 0.f)
                    continue;

                Vector2D screenPos;
                if (WorldToScreen(validLocation, Globals::pRefDef, &screenPos))
                {
                    int fdistance = sqrt(pow(vLocalPos.x - validLocation.x, 2) + pow(vLocalPos.y - validLocation.y, 2) + pow(vLocalPos.z - validLocation.z, 2)) * 0.0254f;

                    if (fdistance > 300)
                        continue;
                    
                    ImColor color{ 20, 200, 30, 255 };
                    if (fdistance <= 100)
                        color = { 80, 127, 255, 255 };

                    std::string sdistance = "[" + std::to_string(fdistance); sdistance.append("m]");
                    std::string text = "Player " + sdistance;

                    if (Settings::bTextToggle)
                        g_Renderer->RenderText(text, ImVec2(screenPos.x-15, screenPos.y), 18, color);

                    if (Settings::bSnapLinesToggle)
                        g_Renderer->RenderLine(ImVec2(960, 1080), ImVec2(screenPos.x, screenPos.y), color);
                }
            }
            else if (type == 14 && Settings::bTextToggle) // vehicle
            {
                Vector entpos = pEntity->location;
                if (entpos.x < 2.f)
                    continue;

                Vector2D screenPos;
                if (WorldToScreen(entpos, Globals::pRefDef, &screenPos))
                {
                    Vector vLocalPos = Globals::GetLocalPlayerPos();
                    int fdistance = sqrt(pow(vLocalPos.x - entpos.x, 2) + pow(vLocalPos.y - entpos.y, 2) + pow(vLocalPos.z - entpos.z, 2)) * 0.0254f;

                    if (fdistance > 600)
                        continue;

                    std::string sdistance = "[" + std::to_string(fdistance); sdistance.append("m]");
                    std::string text = "Vehicle " + sdistance;
                    g_Renderer->RenderText(text, ImVec2(screenPos.x-15, screenPos.y), 18, ImColor(80, 70, 200, 255));
                }
            }        
        }
}
