#pragma once
#include "header.h"
#include "Windows.h"
#include <iostream>
#include "header.h"
#include "../globals/params.h"


std::map<int, time_t> gl_params::apm_map{};
int gl_params::count = 0;
bool gl_params::eject;
int gl_params::apm;
time_t gl_params::cur_time;
std::map<int, time_t> gl_params::cps_map{};
int gl_params::cps;
int gl_params::count2 = 0;


int keyPressed(int key)
{
    return (GetAsyncKeyState(key) & 0x8000 != 0);
}

void actions()
{
    while (1)
    {
        for (int x = 0; x < 255; x++)
        {
            if (keyPressed(x))
            {
                /*
                if (x == 0x01 || x == 0x02)
                {
                    gl_params::count2;
                    gl_params::apm_map[gl_params::count2] = gl_params::cur_time;
                } */
                gl_params::count++;
                gl_params::apm_map[gl_params::count] = gl_params::cur_time;
            }
        }
    }
}


void update_apm()
{
    while (1)
    {
        gl_params::apm = gl_params::apm_map.size();
    }
}


void timer()
{
    while (1)
    {
        gl_params::cur_time = time(NULL);
    }
}

void check_map_apm()
{
    while (1)
    {
        std::map<int, time_t>::iterator it = gl_params::apm_map.begin();
        if (gl_params::apm_map.size() != 0)
        {
            while (it != gl_params::apm_map.end()) {
                if (it->second + 60 < gl_params::cur_time) {
                    gl_params::apm_map.erase(it++);
                }
                else {
                    ++it;
                }
            }
        }
    }
}

void update_cps()
{
    while (1)
    {
        gl_params::apm = gl_params::cps_map.size();
    }
}

void check_map_cps()
{
    while (1)
    {
        std::map<int, time_t>::iterator it = gl_params::cps_map.begin();
        if (gl_params::cps_map.size() != 0)
        {
            while (it != gl_params::cps_map.end()) {
                if (it->second + 2 < gl_params::cur_time) {
                    gl_params::cps_map.erase(it++);
                }
                else {
                    ++it;
                }
            }
        }
    }
}

void get_cps()
{
    while (1)
    {
        for (int x = 0; x < 3; x++)
        {
            if (keyPressed(x))
            {
                gl_params::count2;
                gl_params::apm_map[gl_params::count2] = gl_params::cur_time;
            }
        }
    }
}