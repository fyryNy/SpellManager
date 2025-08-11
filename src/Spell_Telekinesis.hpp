namespace GOTHIC_NAMESPACE
{
    int CurrentTelStep = 0;

    void oCSpell::StopTelekinesis_Union(zCVob* vob)
    {
        if(!vob)
        {
            return;
        }

        if(!vob->GetHomeWorld())
        {
            return;
        }

        auto groundVec = zVEC3(0.0f, -(vob->GetPositionWorld()[VY] - vob->bbox3D.mins[VY]), 0.0f);
        if(!vob->GetHomeWorld()->TraceRayNearestHit(vob->GetPositionWorld(), groundVec + zVEC3(0.0f, -1.0f, 0.0f), vob, zTRACERAY_VOB_IGNORE_CHARACTER | zTRACERAY_VOB_IGNORE_NO_CD_DYN))
        {
            vob->SetPhysicsEnabled(TRUE);
            vob->GetRigidBody()->gravityOn = TRUE;
            vob->SetSleeping(FALSE);
            vob->GetRigidBody()->SetVelocity(zVEC3(0.0f, -100.0f, 0.0f));
        }

        auto item = zDYNAMIC_CAST<oCItem>(vob);
        if(item && this->spellCasterNpc->GetDistanceToVob(*vob) <= 250.0f)
        {
            auto msg = new oCMsgManipulate(oCMsgManipulate::EV_TAKEVOB, item);
            this->spellCasterNpc->GetEM()->OnMessage(msg, this->spellCasterNpc);
        }
    }

    void oCSpell::InvestTelekinesis_Union()
    {
        if(!this->spellCasterNpc)
        {
            return;
        }

        if(!this->spellTarget)
        {
            return;
        }

        if(this->manaInvested <= 0)
        {
            this->spellCasterNpc->CheckForOwner(this->spellTarget);
        }

        if(!this->spellCasterNpc->GetModel()->IsAniActive_Union("S_TELSHOOT"))
        {
            return;
        }

        CurrentTelStep = this->manaInvested;

        float speed = 50.0f / 1000.0f;

        auto move = zVEC3(0.0f, 0.0f, 0.0f);

        auto add = this->spellTarget->GetPositionWorld() - this->spellCasterNpc->GetPositionWorld();
        add[VY] = 0.0f;
        add.NormalizeApprox();

        auto right = zVEC3(0.0f, 1.0f, 0.0f) ^ add;

        if(zinput->GetState(GAME_UP))
        {
            if((this->spellTarget->GetPositionWorld() - this->spellCasterNpc->GetPositionWorld()).LengthApprox() > 200.0f)
            {
                move += -add;
            }
            else
            {
                this->SetReleaseStatus();
                return;
            }
        }

        /*if(zinput->GetState(GAME_LEFT) || zinput->GetState(GAME_STRAFELEFT))
        {
            move += -right;
        }
        else if(zinput->GetState(GAME_RIGHT) || zinput->GetState(GAME_STRAFERIGHT))
        {
            move += right;
        }

        if(zinput->GetState(GAME_SLOW))
        {
            move += zVEC3(0.0f, 1.0f, 0.0f);
        }
        else if(zinput->GetState(GAME_SMOVE))
        {
            move += zVEC3(0.0f, -1.0f, 0.0f);
        }*/

        float fac = speed * ztimer->frameTimeFloat;

        auto itemPos = this->spellTarget->GetPositionWorld();
        auto npcPos = this->spellCasterNpc->bbox3D.maxs;

        if(itemPos[VY] < npcPos[VY])
        {
            move += zVEC3(0.0f, 1.0f, 0.0f);
            this->up += fac;
        }

        itemPos += move * fac;

        if(itemPos[VY] >= npcPos[VY])
        {
            this->hoverY += this->hoverDir * ztimer->frameTimeFloat * speed / 4;

            if(this->hoverY <= -5.0f || this->hoverY >= 5.0f)
            {
                this->hoverDir = -this->hoverDir;
            }

            move[VY] += (this->hoverY - hoverOld);
            this->hoverOld = this->hoverY;
        }

        auto curPos = itemPos + move;

        int inMove = this->spellCasterNpc->isInMovementMode;
        if(inMove)
        {
            this->spellCasterNpc->EndMovement(
            #if ENGINE == Engine_G2A
                TRUE
            #endif
            );
        }

        this->spellTarget->SetPositionWorld(curPos);

        if(inMove)
        {
            this->spellCasterNpc->BeginMovement();
        }
    }
}