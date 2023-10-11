#include <cmath>
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
using namespace std;

int main()
{
    // to play 1 second we need array of 44100 numbers
    const int sampleRate = 44100;
    // create a typed array of size 44100 float numbers
    int16_t sineWaveArray[sampleRate];

    const int hz = 440;
    // fill all 44100 elements of array with Math.sin() values
    for (int i = 0; i < sampleRate; i++) {
        // cout<<",i:"<<i<<",value:"<<sin(i * M_PI * 2 / hz);
        sineWaveArray[i] = 0x4000000 * sin (i * M_PI * 2 / hz);
    }

    // for (int i = 0; i < sampleRate; i++) {
    //     cout<<" "<<sineWaveArray[i]<<endl;
    // }

    sf::SoundBuffer buf;
    buf.loadFromSamples(sineWaveArray, sampleRate, 1, sampleRate);
    // buf.loadFromFile("nice_music.ogg");
    sf::Sound sound;
    sound.setBuffer(buf);
    sound.play();

    while(sound.getStatus() == sf::Sound::Playing)
    {
        sf::sleep(sf::seconds(1.f));
    }
}