#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <winable.h>
#include <math.h>

#include "wiimote.h"
#include "myplayer.h"

int PressKey(byte key) {
    keybd_event(key,0,0,0);
}

int ReleaseKey(byte key) {
    keybd_event(key,0,KEYEVENTF_KEYUP,0);
}

int main()
{
	bool quit=0;
	int nwm=0;
	float gfx=0, gfy=0, gfz=0;
	float gfxp=0, gfyp=0, gfzp=0;

	float ngfx=0, ngfy=0, ngfz=0;
	float ngfxp=0, ngfyp=0, ngfzp=0;

	float wbr=0,wbl=0;
	float wbrp=0,wblp=0;

	bool nconnected = 0;
	bool wbconnected = 0;

	int wmdelay = 0;
	int nkdelay = 0;
	int bbdelay = 0;

	InitWiimotes(4);
    system("cls");
    nwm = (int)GetNumWiimotes();
    UpdateWiimotes();

    Media::mciOpen("Snare2.wav","snare");
    Media::mciOpen("Crash.wav","crash");
    Media::mciOpen("Bass3.wav","bass");
    Media::mciOpen("Ride.wav","ride");
    Media::mciOpen("HiHat6.wav","hihat");

    for (int i=0;i<nwm;i++) {
        SetWiimoteLED(i,1,0,0,0);
        RumbleWiimote(i,1);
        Sleep(100);
        RumbleWiimote(i,0);
        UseMotionSensing(i,1);
    }

    float battery = GetWiimoteBattery(0);

    if (nwm>0) {
        while(!quit) {
            UpdateWiimotes();
            for (int i=0;i<nwm;i++) {
                if (GetExpansion(i)==EXP_NUNCHUK && !nconnected) {
                    printf("Nunchuk Connected\n");
                    nconnected = 1;
                }

                //If using the wii balance board
                if (GetExpansion(i)==EXP_WII_BOARD) {
                    if (!wbconnected) {
                        printf("Wii Board Detected\n");
                        wbconnected = 1;
                    }

                    wbrp = wbr;
                    wblp = wbl;

                    wbr = (GetBalanceBoardTR(i)+GetBalanceBoardBR(i))/2;
                    wbl = (GetBalanceBoardTL(i)+GetBalanceBoardBL(i))/2;

                    if (wbrp-wbr<-5 && bbdelay==0) {
                        Media::mciPlay(1,"bass");
                        bbdelay = 5;
                    }

                    if (bbdelay>0) bbdelay--;

                    continue;
                }

                gfxp = gfx;
                gfyp = gfy;
                gfzp = gfz;
                gfx = GetGForceX(i);
                gfx = GetGForceY(i);
                gfz = GetGForceZ(i);

                ngfxp = ngfx;
                ngfyp = ngfy;
                ngfzp = ngfz;
                ngfx = GetNunchukGForceX(i);
                ngfy = GetNunchukGForceY(i);
                ngfz = GetNunchukGForceZ(i);

                if (gfzp-gfz<-1 && wmdelay==0) {
                    if (GetWiimoteKey(i,WM_B)) Media::mciPlay(1,"crash");
                    if (GetWiimoteKey(i,WM_A)) {
                        int vol = (int)((-(gfzp-gfz)-1.0)*10.0);
                        printf("HIT: %i\n",vol);
                        Media::setVolume(vol,"snare");
                        Media::mciPlay(1,"snare");
                    }
                    else Media::mciPlay(1,"hihat");
                    wmdelay = 5;
                }

                if (ngfzp-ngfz<-.2 && nkdelay==0) {
                    Media::mciPlay(1,"snare");
                    nkdelay = 5;
                }

                if (wmdelay>0)wmdelay--;
                if (nkdelay>0)nkdelay--;

                if (GetWiimoteKey(i,WM_HOME)) quit = 1;
            }
        }

        SetWiimoteLED(0,0,0,0,0);
        Destroy();
        return 0;
    }

    printf("NO WIIMOTES FOUND\n");

	return 0;
}
