/*
    Description:
    The interface of Timer module
    用chrono实现的简易限帧 + 计算ms

    Created Date:
    5/21/2022 12:00:00 PM

    Notes:
*/

export module Timer;

import std;
import <stdint.h>;

export class Timer
{
    typedef std::chrono::steady_clock Clock;
    typedef std::chrono::duration<double,std::milli> MS;
    typedef std::chrono::time_point<Clock,MS> Time;

public:
    static double GetMS() {return elapsedTime;}
    static void SetFramerate(uint32_t rate){framerate = rate;}

    static bool Tick()
    {
		Time newFrame = std::chrono::steady_clock::now();
		MS elapsed = newFrame - lastFrame;
		lastFrame = newFrame;
		double t = elapsed.count();

        if (framerate == 0)
        {
            elapsedTime = t;
            return true;
        }
        else
        {
            if (reset)
            {
                elapsedTime = t;
                reset = false;
            }
            else elapsedTime += t;
          
            if (elapsedTime > (1000.0 / double(framerate)))
            {
                reset = true;
                return true;
            }
            else return false;
        }
    }

private:
    static inline Time lastFrame = std::chrono::steady_clock::now();

    // Framerate
    static inline uint32_t framerate = 0;
    static inline bool reset = false;
    static inline double elapsedTime = 0.0;
};