#include "qbot.hpp"

namespace qBot
{
    int frame;
    float xpos;
    bool inLevel = false;

    bool p1ButtonPushed = false;
    bool p2ButtonPushed = false;
    std::vector<std::tuple<bool, bool, float, float, float, float, float, float, double, double, double, double, double, double, double, double>> vanilaMacro;
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
                self->m_pPlayer2->getPositionX(),
                self->m_pPlayer1->getPositionY(),
                self->m_pPlayer2->getPositionY(),
                self->m_pPlayer1->getRotation(),
                self->m_pPlayer2->getRotation(),
                self->m_pPlayer1->m_yAccel,
                self->m_pPlayer2->m_yAccel,
                self->m_pPlayer1->m_xAccel,
                self->m_pPlayer2->m_xAccel,
                self->m_pPlayer1->m_jumpAccel,
                self->m_pPlayer2->m_jumpAccel,
                self->m_pPlayer1->m_gravity,
                self->m_pPlayer2->m_gravity
                });
            }
        }
        
        if (GUI::mode == 2)
        {
            if (frame != 0 && vanilaMacro.size() > (size_t)frame)
            {
                if (self->m_pPlayer1)
                {
                    self->m_pPlayer1->setPositionX(std::get<2>(qBot::vanilaMacro[frame - 1]));
                    self->m_pPlayer1->setPositionY(std::get<4>(qBot::vanilaMacro[frame - 1]));
                    self->m_pPlayer1->setRotation(std::get<6>(qBot::vanilaMacro[frame - 1]));
                    self->m_pPlayer1->m_yAccel = std::get<8>(qBot::vanilaMacro[frame - 1]);
                    self->m_pPlayer1->m_xAccel = std::get<10>(qBot::vanilaMacro[frame - 1]);
                    self->m_pPlayer1->m_jumpAccel = std::get<12>(qBot::vanilaMacro[frame - 1]);
                    self->m_pPlayer1->m_gravity = std::get<14>(qBot::vanilaMacro[frame - 1]);
                    

                    if (std::get<0>(qBot::vanilaMacro[frame]) && !p1ButtonPushed)
                    {
                        Clickbot::PushButton(true);
                        Hooks::PlayLayer::pushButton(self, 0, true);

                        p1ButtonPushed = true;
                    }
                    if (!std::get<0>(qBot::vanilaMacro[frame]) && p1ButtonPushed) {
                        
                        Clickbot::ReleaseButton(true);
                        Hooks::PlayLayer::releaseButton(self, 0, true);
                        
                        p1ButtonPushed = false;
                    }
                }


                if (self->m_pPlayer2)
                {
                    self->m_pPlayer2->setPositionX(std::get<3>(qBot::vanilaMacro[frame - 1]));
                    self->m_pPlayer2->setPositionY(std::get<5>(qBot::vanilaMacro[frame - 1]));
                    self->m_pPlayer2->setRotation(std::get<7>(qBot::vanilaMacro[frame - 1]));
                    self->m_pPlayer2->m_yAccel = std::get<9>(qBot::vanilaMacro[frame - 1]);
                    self->m_pPlayer2->m_xAccel = std::get<11>(qBot::vanilaMacro[frame - 1]);
                    self->m_pPlayer2->m_jumpAccel = std::get<13>(qBot::vanilaMacro[frame - 1]);
                    self->m_pPlayer2->m_gravity = std::get<15>(qBot::vanilaMacro[frame - 1]);


                    if (std::get<1>(qBot::vanilaMacro[frame]) && !p2ButtonPushed)
                    {
                        Clickbot::PushButton(false);
                        Hooks::PlayLayer::pushButton(self, 0, false);
                        
                        p2ButtonPushed = true;
                    }
                    if (!std::get<1>(qBot::vanilaMacro[frame]) && p2ButtonPushed) {
                        
                        Clickbot::ReleaseButton(false);
                        Hooks::PlayLayer::releaseButton(self, 0, false);

                        p2ButtonPushed = false;
                    }
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