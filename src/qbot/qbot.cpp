#include "qbot.hpp"

namespace qBot
{
    int frame;
    float xpos;
    bool inLevel = false;
    bool accuracyFixEnabled = false;
    bool showStatusEnabled = false;
    bool fakeCheatIndicatorEnabled = false;

    bool p1ButtonPushed = false;
    bool p2ButtonPushed = false;

    bool frameAdvanceEnabled = false;
    bool advanceOneFrame = false;

    std::vector<std::tuple<bool, bool, float, float, float, float, float, float, double, double, bool, bool>> vanilaMacro;
    std::vector<int> checkpoints;
    std::string levelName = "";

    std::string getStatusText()
    {
        std::stringstream ss;
        switch (GUI::mode)
        {
        case 1:
            ss << "Recording: " << frame;
            break;
        
        case 2:
            ss << "Playing: " << frame + 1 << "/" << vanilaMacro.size();
            break;

        default:
            break;
        }
        return ss.str();
    }


    void Update(gd::PlayLayer* self)
    {
        auto statusText = cast<CCLabelBMFont*>(self->getChildByTag(4000));
        auto cheatIndicator = cast<CCLabelBMFont*>(self->getChildByTag(3999));
        xpos = self->m_pPlayer1->getPositionX();
        
        if (self->m_pPlayer1->getPositionX() == 0)
        {
            frame = 0;
        } else if (!self->m_isDead) {
            frame++;
        }

        if (fakeCheatIndicatorEnabled)
        {
            cheatIndicator->setVisible(true);
        } else {
            cheatIndicator->setVisible(false);
        }
        

        if (GUI::mode == 0)
        {
            statusText->setVisible(false);
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
                self->m_pPlayer1->unk5FE,
                self->m_pPlayer2->unk5FE,
                });
            }
            if (showStatusEnabled)
            {
                statusText->setString(getStatusText().c_str());
                statusText->setVisible(true);
                auto size = statusText->getScaledContentSize();
                statusText->setPosition({ size.width / 2 + 3, size.height / 2 + 3});
            } else {
                statusText->setVisible(false);
            }
        }
        
        if (GUI::mode == 2)
        {
            
            if (frame != 0 && vanilaMacro.size() > (size_t)frame && !self->m_isDead)
            {
                if (std::get<0>(qBot::vanilaMacro[frame]) && !p1ButtonPushed) {
                    Hooks::PlayLayer::pushButton(self, 0, true);
                    p1ButtonPushed = true;
                }
                if (!std::get<0>(qBot::vanilaMacro[frame]) && p1ButtonPushed) {
                    Hooks::PlayLayer::releaseButton(self, 0, true);
                    p1ButtonPushed = false;
                }
                if (accuracyFixEnabled)
                {
                    self->m_pPlayer1->setPositionX(std::get<2>(qBot::vanilaMacro[frame]));
                    self->m_pPlayer1->setPositionY(std::get<4>(qBot::vanilaMacro[frame]));
                    self->m_pPlayer1->setRotation(std::get<6>(qBot::vanilaMacro[frame]));
                }

                if (std::get<1>(qBot::vanilaMacro[frame]) && !p2ButtonPushed) {
                    Hooks::PlayLayer::pushButton(self, 0, false);
                    p2ButtonPushed = true;
                }
                if (!std::get<1>(qBot::vanilaMacro[frame]) && p2ButtonPushed) {
                    Hooks::PlayLayer::releaseButton(self, 0, false);
                    p2ButtonPushed = false;
                }
                
                if (accuracyFixEnabled)
                {
                    self->m_pPlayer2->setPositionX(std::get<3>(qBot::vanilaMacro[frame]));
                    self->m_pPlayer2->setPositionY(std::get<5>(qBot::vanilaMacro[frame]));
                    self->m_pPlayer2->setRotation(std::get<7>(qBot::vanilaMacro[frame]));
                }
                

                if (showStatusEnabled)
                {
                    statusText->setString(getStatusText().c_str());
                    statusText->setVisible(true);
                    auto size = statusText->getScaledContentSize();
                    statusText->setPosition({ size.width / 2 + 3, size.height / 2 + 3});
                } else {
                    statusText->setVisible(false);
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
        if (!self->m_isPracticeMode && GUI::mode == 1)
        {
            vanilaMacro.clear();
        }
        
        if (checkpoints.size() != 0 && vanilaMacro.size() != 0)
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
        frame = 0;
        const auto winSize = CCDirector::sharedDirector()->getWinSize();

        CCLabelBMFont* statusText = CCLabelBMFont::create("Bot Status", "chatFont.fnt");
        statusText->setZOrder(1000);
        statusText->setTag(4000);
        auto size = statusText->getScaledContentSize();
        statusText->setPosition({ size.width / 2 + 3, size.height / 2 + 3});
        self->addChild(statusText);

        CCLabelBMFont* cheatIndicator = CCLabelBMFont::create(".", "bigFont.fnt");
        cheatIndicator->setColor({0, 255, 0});
        cheatIndicator->setAnchorPoint({0.f, 0.5f});
        cheatIndicator->setTag(3999);
        cheatIndicator->setPosition({5.f, winSize.height});
        cheatIndicator->setZOrder(99999);
        self->addChild(cheatIndicator);
    }

    void Quit(gd::PlayLayer* self)
    {
        levelName = "";
        xpos = 0;
        frame = 0;
        inLevel = false;
        checkpoints.clear();
    }

    void Death()
    {

    }
} // namespace qBot