#include "qbot.hpp"

namespace qBot
{
    int frame;
    float xpos;
    bool inLevel = false;
    bool accuracyFixEnabled = false;
    bool checkpointFixEnabled = false;
    bool showStatusEnabled = false;
    bool fakeCheatIndicatorEnabled = false;

    size_t index;

    bool p1ButtonPushed = false;
    bool p2ButtonPushed = false;

    bool frameAdvanceEnabled = false;
    bool advanceOneFrame = false;

    std::vector<Click> macro;
    std::vector<Checkpoint> checkpoints;
    std::vector<nfdchar_t*> sequence;

    bool playingSequence = false;
    int sequenceIndex = 0;

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
            ss << "Playing: " << index << "/" << macro.size() - 1;
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
            if (showStatusEnabled)
            {
                statusText->setString(getStatusText().c_str());
                statusText->setVisible(true);
                auto size = statusText->getScaledContentSize();
                statusText->setPosition({ size.width / 2 + 3, size.height / 2 + 3});
            } else {
                statusText->setVisible(false);
            }

            if (frame != 0 && !self->m_isDead)
            {
                for (size_t i = 0; i < macro.size(); i++) {
                    while (macro[i].frame == frame)
                    {
                        index = i;
                        if (macro[i].action)
                        {
                            std::cout << macro[i].xpos << std::endl;
                            if (macro[i].xpos != -1 && qBot::accuracyFixEnabled)
                            {
                                self->m_pPlayer1->setPositionX(macro[i].xpos);
                                self->m_pPlayer2->setPositionX(macro[i].xpos);
                                self->m_pPlayer1->setPositionY(macro[i].ypos1);
                                self->m_pPlayer2->setPositionY(macro[i].ypos2);
                                self->m_pPlayer1->setRotation(macro[i].rotation1);
                                self->m_pPlayer2->setRotation(macro[i].rotation2);
                            }
                            Hooks::PlayLayer::pushButton(self, 0, macro[i].player);
                        } else {
                            if (macro[i].xpos != -1 && qBot::accuracyFixEnabled)
                            {
                                self->m_pPlayer1->setPositionX(macro[i].xpos);
                                self->m_pPlayer2->setPositionX(macro[i].xpos);
                                self->m_pPlayer1->setPositionY(macro[i].ypos1);
                                self->m_pPlayer2->setPositionY(macro[i].ypos2);
                                self->m_pPlayer1->setRotation(macro[i].rotation1);
                                self->m_pPlayer2->setRotation(macro[i].rotation2);
                            }
                            Hooks::PlayLayer::releaseButton(self, 0, macro[i].player);
                        }
                        break;
                    }
                }
            }
        }
    }

    void PushButton(gd::PlayLayer* self, bool player)
    {
        if (GUI::mode == 1 && frame != 0)
        {
            if (qBot::accuracyFixEnabled)
            {
                macro.push_back({(int)FPSMultiplier::target_fps, frame,
                                 self->m_pPlayer1->getPositionX(),
                                 self->m_pPlayer1->getPositionY(),
                                 self->m_pPlayer2->getPositionY(),
                                 self->m_pPlayer1->getRotation(),
                                 self->m_pPlayer2->getRotation(),
                                 true, player});
            } else {
                macro.push_back({(int)FPSMultiplier::target_fps, frame, -1, -1, -1, -1, -1, true, player});
            }
        }
        
    }

    void ReleaseButton(gd::PlayLayer* self, bool player)
    {
        if (GUI::mode == 1 && frame != 0)
        {
            if (qBot::accuracyFixEnabled)
            {
                macro.push_back({(int)FPSMultiplier::target_fps, frame,
                                 self->m_pPlayer1->getPositionX(),
                                 self->m_pPlayer1->getPositionY(),
                                 self->m_pPlayer2->getPositionY(),
                                 self->m_pPlayer1->getRotation(),
                                 self->m_pPlayer2->getRotation(),
                                 false, player});
            } else {
                macro.push_back({(int)FPSMultiplier::target_fps, frame, -1, -1, -1, -1, -1, false, player});
            }

        }
    }

    void PlaceCheckpoint(gd::PlayLayer* self)
    {
        checkpoints.push_back({frame,
                               self->m_pPlayer1->getPositionX(),
                               self->m_pPlayer1->getPositionY(),
                               self->m_pPlayer2->getPositionY(),
                               self->m_pPlayer1->getRotation(),
                               self->m_pPlayer2->getRotation(),
                               self->m_pPlayer1->m_yAccel,
                               self->m_pPlayer2->m_yAccel
        });
    }

    void RemoveCheckpoint(gd::PlayLayer* self)
    {
        checkpoints.pop_back();
    }

    void Reset(gd::PlayLayer* self)
    {
        index = 0;

        if (self->m_isPracticeMode && checkpoints.size() != 0 && macro.size() != 0)
        {
            frame = checkpoints.back().frame;
            if (checkpointFixEnabled)
            {
                self->m_pPlayer1->setPositionX(checkpoints.back().xpos);
                self->m_pPlayer2->setPositionX(checkpoints.back().xpos);
                self->m_pPlayer1->setPositionY(checkpoints.back().ypos1);
                self->m_pPlayer2->setPositionY(checkpoints.back().ypos2);
                self->m_pPlayer1->setRotation(checkpoints.back().rotation1);
                self->m_pPlayer2->setRotation(checkpoints.back().rotation2);
                self->m_pPlayer1->m_yAccel = checkpoints.back().yAccel1;
                self->m_pPlayer2->m_yAccel = checkpoints.back().yAccel2;
            }
            while (macro.back().frame > checkpoints.back().frame)
            {
                macro.pop_back();
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
        index = 0;
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