#include "qbot.hpp"

namespace qBot
{
    int frame;
    float xpos;
    bool inLevel = false;

    bool p1ButtonPushed = false;
    bool p2ButtonPushed = false;
    std::vector<std::tuple<bool, bool, float, float>> vanilaMacro;
    std::vector<int> checkpoints;
    std::string levelName = "";
    
    void Update(gd::PlayLayer* self)
    {
        xpos = self->m_pPlayer1->getPositionX();
        
        if (self->m_pPlayer1->getPositionX() == 0)
        {
            frame = 0;
        } else if (!self->m_isDead && !self->m_hasLevelCompleteMenu) {
            frame++;
        }


        if (GUI::mode == 1)
        {
            if (frame != 0)
            {
                vanilaMacro.push_back({
                p1ButtonPushed,
                p2ButtonPushed,
                self->m_pPlayer1->getPositionX(),
                self->m_pPlayer2->getPositionX()
                });
            }
        }
        
        if (GUI::mode == 2)
        {
            if (frame != 0 && vanilaMacro.size() > (size_t)frame)
            {
                
                self->m_pPlayer1->setPositionX(std::get<2>(qBot::vanilaMacro[frame - 1]));

                if (std::get<0>(qBot::vanilaMacro[frame]) && !p1ButtonPushed)
                {
                    Hooks::PlayLayer::pushButton(self, 0, true);
                    p1ButtonPushed = true;
                }
                if (!std::get<0>(qBot::vanilaMacro[frame]) && p1ButtonPushed) {
                    Hooks::PlayLayer::releaseButton(self, 0, true);
                    p1ButtonPushed = false;
                }

                
                self->m_pPlayer2->setPositionX(std::get<3>(qBot::vanilaMacro[frame - 1]));

                if (std::get<1>(qBot::vanilaMacro[frame]) && !p2ButtonPushed)
                {
                    Hooks::PlayLayer::pushButton(self, 0, false);
                    p2ButtonPushed = true;
                }
                if (!std::get<1>(qBot::vanilaMacro[frame]) && p2ButtonPushed) {
                    Hooks::PlayLayer::releaseButton(self, 0, false);
                    p2ButtonPushed = false;
                }
            }
        }
    }

    void PushButton(gd::PlayLayer* self, bool player)
    {
        if (GUI::mode == 1)
        {
            if (player)
            {
                p1ButtonPushed = true;
            } else {
                p2ButtonPushed = true;
            }
        }
    }

    void ReleaseButton(gd::PlayLayer* self, bool player)
    {
        if (GUI::mode == 1)
        {
            if (player)
            {
                p1ButtonPushed = false;
            } else {
                p2ButtonPushed = false;
            }
        }
    }

    void PlaceCheckpoint()
    {
        checkpoints.push_back(frame);
    }

    void RemoveCheckpoint()
    {
        checkpoints.pop_back();
    }

    void Reset(gd::PlayLayer* self)
    {
        if (!self->m_isPracticeMode)
        {
            Hooks::PlayLayer::releaseButton(self, 0, true);
            Hooks::PlayLayer::releaseButton(self, 0, false);
            
            frame = 0;
        }

        if (GUI::mode == 1 && !self->m_isPracticeMode)
        {
            qBot::vanilaMacro.clear();
            frame = 0;
        }
        

        if (GUI::mode == 1 && self->m_isPracticeMode && checkpoints.size() != 0 && vanilaMacro.size() != 0)
        {
            frame = checkpoints.back();
            while (vanilaMacro.size() > (size_t)checkpoints.back())
            {
                vanilaMacro.pop_back();
            }
        }
    }

    void Init(gd::PlayLayer* self)
    {
        levelName = self->m_level->m_sLevelName;
        inLevel = true;
    }

    void Quit(gd::PlayLayer* self)
    {
        levelName = "";
        xpos = 0;
        frame = 0;
        inLevel = false;
    }
} // namespace qBot