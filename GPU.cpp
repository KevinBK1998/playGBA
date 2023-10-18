#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "FailureCodes.h"
#include "GPU.h"
#include "CommonDS.h"

using namespace std;

GPU::GPU(sf::RenderWindow* window){
    gameWindow = window;
    gameWindow->setPosition(sf::Vector2i(0,0));
    gameWindow->clear(sf::Color::Black);
    gameWindow->display();
}

void GPU::storeHalfWord(int address, uint16_t* var, uint8_t data){
    if (address){
        *(var) &= 0xFF;
        *(var) |= (data<<8);
        status();
    }
    else {
        *(var) &= 0xFF00;
        *(var) |= data;
    }
}

void GPU::storeWord(int address, uint32_t* var, uint8_t data){
    if (address == 0){
        *(var) &= 0xFFFFFF00;
        *(var) |= data;
    }
    else if (address == 1){
        *(var) &= 0xFFFF00FF;
        *(var) |= (data<<8);
    }
    else if (address == 2){
        *(var) &= 0xFF00FFFF;
        *(var) |= (data<<16);
    }
    else {
        *(var) &= 0xFFFFFF;
        *(var) |= (data<<24);
        status();
    }
}

uint8_t GPU::read8(uint32_t address){
    if (address >= VRAM_OFFSET && address < OAM_OFFSET){
        DEBUG_OUT << "R: GPU VRAM Memory: "<<address<<endl;
        return vram[address-VRAM_OFFSET];
    }
    cout << "R: GPU Undefined Memory: "<<address<<endl;
    exit(FAILED_DMA);
}

void GPU::write8(uint32_t address, uint8_t data){
    uint16_t* pointer;
    uint32_t* wordPointer;
    int actualAddress;
    bool isWord=false;
    if (address<PAL_RAM_OFFSET) switch (address & 0xFE)
    {
    case DISPCNT ... DISPCNT+1:
        dispCnt.storeReg(address, data);
        status();
        return;
    case GREENSWAP ... GREENSWAP+1:
        pointer = &greenSwap;
        break;
    case DISPSTAT ... DISPSTAT+1:
        dispStat.storeReg(address, data, 0x47);
        status();
        return;
    case VCOUNT ... VCOUNT+1:
        pointer = &vCount;
        break;
    case BGCNT_OFFSET ... BGCNT_END:
        actualAddress = (address>>1) & 0b11;
        pointer = &bgCnt[actualAddress];
        break;
    case BGHW_OFFSET ... BGHW_OFFSET_END:
        actualAddress = (address>>1) & 0b111;
        pointer = (actualAddress&1)? &bgOffsetY[actualAddress>>1]: &bgOffsetX[actualAddress>>1];
        break;
    case BG2PAR_OFFSET ... BG2PAR_OFFSET_END:
        actualAddress = (address>>1) & 0b11;
        bg2Parameters[actualAddress].storeReg(address, data);
        return;
    case BG2X ... BG2X+3:
        isWord = true;
        wordPointer = &bg2x;
        break;
    case BG2Y ... BG2Y+3:
        isWord = true;
        wordPointer = &bg2y;
        break;
    case BG3PAR_OFFSET ... BG3PAR_OFFSET_END:
        actualAddress = (address>>1) & 0b11;
        bg3Parameters[actualAddress].storeReg(address, data);
        return;
    case BG3X ... BG3X+3:
        isWord = true;
        wordPointer = &bg3x;
        break;
    case BG3Y ... BG3Y+3:
        isWord = true;
        wordPointer = &bg3y;
        break;
    case WIN0H ... WIN0H+1:
        pointer = &win0x;
        break;
    case WIN1H ... WIN1H+1:
        pointer = &win1x;
        break;
    case WIN0V ... WIN0V+1:
        pointer = &win0y;
        break;
    case WIN1V ... WIN1V+1:
        pointer = &win1y;
        break;
    case WININ ... WININ+1:
        pointer = &winIn;
        break;
    case WINOUT ... WINOUT+1:
        pointer = &winOut;
        break;
    case MOSAIC ... MOSAIC+1:
        pointer = &mosaic;
        break;
    case MOSAIC+2 ... MOSAIC+3:
        DEBUG_OUT << "W: GPU Unused Memory: "<<unsigned(address)<<endl;
        return;
    case BLDCNT ... BLDCNT+1:
        pointer = &sfxCnt;
        break;
    case BLDALPHA ... BLDALPHA+1:
        pointer = &alphaCoeff;
        break;
    case BLDY ... BLDY+1:
        pointer = &brightCoeff;
        break;
    case BLDY+2 ... BLDY+11:
        DEBUG_OUT << "W: GPU Unused Memory: "<<unsigned(address)<<endl;
        return;
    default:
        cout << "W: GPU Undefined Memory: "<<unsigned(address)<<endl;
        exit(FAILED_DMA);
    }
    else if (address < VRAM_OFFSET){
        DEBUG_OUT << "W: GPU Palette RAM Memory: "<<unsigned(address)<<endl;
        palRam[address-PAL_RAM_OFFSET]=data;
        return;
    }
    else if (address < OAM_OFFSET){
        DEBUG_OUT << "W: GPU VRAM Memory: "<<unsigned(address)<<endl;
        vram[address-VRAM_OFFSET]=data;
        return;
    }
    else {
        DEBUG_OUT << "W: GPU OAM Memory: "<<unsigned(address)<<endl;
        oam[address-OAM_OFFSET]=data;
        return;
    }
    if(isWord)
        storeWord(address&0b11, wordPointer, data);
    else
        storeHalfWord(address&1, pointer, data);
}

void GPU::status(){
    DEBUG_OUT<<"\nGPU:"<<endl;
    DEBUG_OUT<<"DISPCNT: "<<dispCnt.getRegValue()<<"\tGREENSWAP: "<<greenSwap<<"\tDISPSTAT: "<<dispStat.getRegValue()<<"\tVCOUNT: "<<vCount<<endl;
    DEBUG_OUT<<"BG0CNT: "<<bgCnt[0]<<"\tBG1CNT: "<<bgCnt[1]<<"\tBG2CNT: "<<bgCnt[2]<<"\tBG3CNT: "<<bgCnt[3]<<endl;
    DEBUG_OUT<<"BG0HOFS: "<<bgOffsetX[0]<<"\tBG0HOFS: "<<bgOffsetX[1]<<"\tBG0HOFS: "<<bgOffsetX[2]<<"\tBG0HOFS: "<<bgOffsetX[3]<<endl;
    DEBUG_OUT<<"BG0VOFS: "<<bgOffsetY[0]<<"\tBG0VOFS: "<<bgOffsetY[1]<<"\tBG0VOFS: "<<bgOffsetY[2]<<"\tBG0VOFS: "<<bgOffsetY[3]<<endl;
    DEBUG_OUT<<"BG2PA: "<<bg2Parameters[0].getRegValue()<<"\tBG2PB: "<<bg2Parameters[1].getRegValue()<<"\tBG2PC: "<<bg2Parameters[2].getRegValue()<<"\tBG2PD: "<<bg2Parameters[3].getRegValue()<<"\tBG2X: "<<bg2x<<"\tBG2Y: "<<bg2y<<endl;
    DEBUG_OUT<<"BG3PA: "<<bg3Parameters[0].getRegValue()<<"\tBG3PB: "<<bg3Parameters[1].getRegValue()<<"\tBG3PC: "<<bg3Parameters[2].getRegValue()<<"\tBG3PD: "<<bg3Parameters[3].getRegValue()<<"\tBG3X: "<<bg3x<<"\tBG3Y: "<<bg3y<<endl;
    DEBUG_OUT<<"WIN0H: "<<win0x<<"\tWIN1H: "<<win1x<<"\tWIN0V: "<<win0y<<"\tWIN1V: "<<win1y<<"\tWININ: "<<winIn<<"\tWINOUT: "<<winOut<<endl;
    DEBUG_OUT<<"MOSAIC: "<<mosaic<<"\tBLDCNT: "<<sfxCnt<<"\tBLDALPHA: "<<alphaCoeff<<"\tBLDY: "<<brightCoeff<<endl<<endl;
}

void GPU::dump(){
    dumpArrayToFile(vram, VRAM_SIZE, "vram.bin");
    dumpArrayToFile(oam, OAM_SIZE, "oam.bin");
    dumpArrayToFile(palRam, PAL_RAM_SIZE, "palRam.bin");
}

sf::VertexArray fillColor(sf::Color clearColor){
    sf::VertexArray screen(sf::Triangles, 6);
    // Position
    screen[0].position = sf::Vector2f(0, 0);
    screen[1].position = sf::Vector2f(SCREEN_WIDTH, 0);
    screen[2].position = sf::Vector2f(0, SCREEN_HEIGHT);
    screen[3].position = sf::Vector2f(0, SCREEN_HEIGHT);
    screen[4].position = sf::Vector2f(SCREEN_WIDTH, 0);
    screen[5].position = sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Color
    screen[0].color = clearColor;
    screen[1].color = clearColor;
    screen[2].color = clearColor;
    screen[3].color = clearColor;
    screen[4].color = clearColor;
    screen[5].color = clearColor;
    return screen;
}

void GPU::step(){
    timer++;
    DEBUG_OUT<<"GPU:"<<timer<<endl;
    DEBUG_OUT<<"GPUc:"<<timerC<<endl;
    if (timer > 100000){
        if (dispStat.bitCheck(3)){
            dispStat.storeReg(0, 1, 0xFFFE);
            cout<<"VBLANKING"<<endl;
        }
        timer=0;
        if (gameWindow->isOpen())
        {
            sf::Event event;
            while (gameWindow->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    gameWindow->close();
            }

            gameWindow->clear(sf::Color::Black);
            sf::Color color = sf::Color(timerC, timerC, timerC, 0xFF);
            timerC++;
            gameWindow->draw(fillColor(color));
            gameWindow->display();
        }
    }
}

bool GPU::checkVBlankInterruptFlag(){
    return dispStat.bitCheck(0);
}
