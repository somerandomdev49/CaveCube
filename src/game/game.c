#include "main.h"
#include "game.h"
#include <common.h>
#include <resource.h>
#include <bmd.h>
#include <renderer.h>
#include <input.h>
#include <noise.h>
#include <chunk.h>
#include <server.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#ifndef SHOWFPS
    //#define SHOWFPS
#endif

static float posmult = 0.125;
static float rotmult = 3;
static float fpsmult = 0;
static float mousesns = 0.05;

struct chunkdata chunks;

/*
static inline void renderChunks(struct chunkdata* data, float xrot, float yrot) {
    int yroti;
    if (xrot < -45.0) {
        yroti = 5;
    } else if (xrot > 45.0) {
        yroti = 6;
    } else {
        yroti = ((yrot + 45) / 90);
        yroti %= 4;
        switch (yroti) {
            case 1:;
                yroti = 3;
                break;
            case 2:;
                yroti = 1;
                break;
            case 3:;
                yroti = 2;
                break;
        }
    }
    //printf("[%d]\n", yroti);
    register int w = data->coff;
    int sides = 0;
    uint64_t starttime = altutime();
    for (register int y = 0; y < 256; ++y) {
        for (register int z = -w; z <= w; ++z) {
            for (register int x = -w; x <= w; ++x) {
                //printf("rendering [%d, %d, %d]...\n", x, y, z);
                struct blockdata bdata = getBlock(&chunks, x, y, z);
                if (!bdata.id || !blockinfo[bdata.id].mdl) continue;
                struct blockdata bdata2[6];
                bdata2[0] = getBlock(&chunks, x, y, z + 1);
                bdata2[1] = getBlock(&chunks, x, y, z - 1);
                bdata2[2] = getBlock(&chunks, x - 1, y, z);
                bdata2[3] = getBlock(&chunks, x + 1, y, z);
                bdata2[4] = getBlock(&chunks, x, y + 1, z);
                bdata2[5] = getBlock(&chunks, x, y - 1, z);
                for (int i = 0; i < 6; ++i) {
                    if (i == yroti) continue;
                    if (!bdata2[i].id || !blockinfo[bdata2[i].id].mdl || (blockinfo[bdata2[i].id].alpha && !blockinfo[bdata.id].alpha)) {
                        renderPartAt(blockinfo[bdata.id].mdl, i, (coord_3d){x, y, z}, false);
                        ++sides;
                    }
                }
            }
        }
    }
    printf("time: [%f] [%d]\n", (float)(altutime() - starttime) / 1000000.0, sides);
}
*/

void doGame() {
    char** tmpbuf = malloc(16 * sizeof(char*));
    for (int i = 0; i < 16; ++i) {
        tmpbuf[i] = malloc(4096);
    }
    /*
    for (int i = 1; i < 256; ++i) {
        sprintf(tmpbuf[0], "game/textures/blocks/%d/", i);
        printf("loading block [%d]...\n", i);
        if (resourceExists(tmpbuf[0]) == -1) break;
        for (int j = 0; j < 6; ++j) {
            sprintf(tmpbuf[j], "game/textures/blocks/%d/%d.png", i, j);
        }
        blockinfo[i].mdl = loadModel("game/models/block/default.bmd", (char**)tmpbuf);
        printf("[%d] [%d]\n", blockinfo[i].mdl->renddata->texture->width, blockinfo[i].mdl->renddata->texture->height);
        blockinfo[i].alpha = (blockinfo[i].mdl->renddata->texture->channels == 4);
        blockinfo[i].mdl->pos = (coord_3d){0.0, -0.5, 0.0};
    }
    */
    chunks = allocChunks(9);
    /*
    for (int i = 0; i < 57600; ++i) {
        chunks.data[0][i].id = getRandByte();
    }
    */
    rendinf.campos.y = 67.5;
    initInput();
    float pmult = posmult;
    float rmult = rotmult;
    setRandSeed(91935);
    initNoiseTable();
    /*
    register int w = chunks.coff;
    for (register int y = 0; y < 256; ++y) {
        for (register int z = -w; z <= w; ++z) {
            for (register int x = -w; x <= w; ++x) {
                setBlock(&chunks, x, y, z, (struct blockdata){0, 0, 0});
            }
        }
    }
    for (register int z = -w; z <= w; ++z) {
        for (register int x = -w; x <= w; ++x) {
            //printf("setting [%d, %d]...\n", x, z);
            double s = perlin2d((double)(x) / 16, (double)(z) / 16, 1.0, 1);
            double s2 = perlin2d(((double)(-x - 1) / 160), ((double)(-z - 1) / 160), 1.0, 1);
            for (register int y = 0; y < 4; ++y) {
                setBlock(&chunks, x, y, z, (struct blockdata){7, 0, 0});
            }
            s *= 5;
            s += s2 * 32;
            int si = (int)(float)(s * 2) - 16;
            setBlock(&chunks, x, si, z, (struct blockdata){(si > 12) ? 5 : 3, 0, 0});
            for (register int y = si - 1; y > -1; --y) {
                setBlock(&chunks, x, y, z, (struct blockdata){2, 0, 0});
            }
            setBlock(&chunks, x, 0, z, (struct blockdata){6, 0, 0});
            if (si < 3) si = 3;
            if (!x && !z) rendinf.campos.y += (float)si;
        }
    }
    */
    initServer(SERVER_MODE_SP);
    int cx = 0;
    int cz = 0;
    genChunks(&chunks, cx, cz);
    //bool uccallagain = updateChunks(&chunks);
    uint64_t fpsstarttime2 = altutime();
    uint64_t ptime = fpsstarttime2;
    uint64_t dtime = fpsstarttime2;
    uint64_t ptime2 = fpsstarttime2;
    uint64_t dtime2 = fpsstarttime2;
    #ifdef SHOW_FPS
    uint64_t fpsstarttime = fpsstarttime2;
    int fpsct = 0;
    #endif
    while (!quitRequest) {
        uint64_t starttime = altutime();
        float npmult = 1.0;
        float nrmult = 1.0;
        input_info input = getInput();
        //printf("rend ");
        //fflush(stdout);
        if (input.multi_actions & INPUT_GETMAFLAG(INPUT_ACTION_MULTI_CROUCH)) {
            rendinf.campos.y -= pmult;
            if (rendinf.campos.y < 1.125) rendinf.campos.y = 1.125;
        } else {
            if (rendinf.campos.y < 1.5) rendinf.campos.y = 1.5;
        }
        if (input.multi_actions & INPUT_GETMAFLAG(INPUT_ACTION_MULTI_JUMP)) {
            rendinf.campos.y += pmult;
        }
        if (input.multi_actions & INPUT_GETMAFLAG(INPUT_ACTION_MULTI_RUN)) {
            npmult *= 2.5;
        }
        rendinf.camrot.x += input.mymov * mousesns * ((input.mmovti) ? rotmult : rmult) * nrmult;
        rendinf.camrot.y -= input.mxmov * mousesns * ((input.mmovti) ? rotmult : rmult) * nrmult;
        if (rendinf.camrot.y < 0) rendinf.camrot.y += 360;
        else if (rendinf.camrot.y >= 360) rendinf.camrot.y -= 360;
        //printf("yr: [%f]\n", rendinf.camrot.y);
        if (rendinf.camrot.x > 89.99) rendinf.camrot.x = 89.99;
        if (rendinf.camrot.x < -89.99) rendinf.camrot.x = -89.99;
        float yrotrad = (rendinf.camrot.y / 180 * M_PI);
        float div = fabs(input.zmov) + fabs(input.xmov);
        if (div < 1.0) div = 1.0;
        //printf("div: [%f]\n", div);
        rendinf.campos.z -= (input.zmov * cosf(yrotrad) * ((input.movti) ? posmult : pmult) * npmult) / div;
        rendinf.campos.x += (input.zmov * sinf(yrotrad) * ((input.movti) ? posmult : pmult) * npmult) / div;
        rendinf.campos.x += (input.xmov * cosf(yrotrad) * ((input.movti) ? posmult : pmult) * npmult) / div;
        rendinf.campos.z += (input.xmov * sinf(yrotrad) * ((input.movti) ? posmult : pmult) * npmult) / div;
        //bool setcallagain = false;
        if (rendinf.campos.z > 8.0) {   
            --cz;
            rendinf.campos.z -= 16.0;
            moveChunks(&chunks, 0, 1);
            genChunks(&chunks, cx, cz);
            //uccallagain = updateChunks(&chunks);
            //setcallagain = true;
        } else if (rendinf.campos.z < -8.0) {
            ++cz;
            rendinf.campos.z += 16.0;
            moveChunks(&chunks, 0, -1);
            genChunks(&chunks, cx, cz);
            //uccallagain = updateChunks(&chunks);
            //setcallagain = true;
        } else if (rendinf.campos.x > 8.0) {
            ++cx;
            rendinf.campos.x -= 16.0;
            moveChunks(&chunks, 1, 0);
            genChunks(&chunks, cx, cz);
            //uccallagain = updateChunks(&chunks);
            //setcallagain = true;
        } else if (rendinf.campos.x < -8.0) {
            --cx;
            rendinf.campos.x += 16.0;
            moveChunks(&chunks, -1, 0);
            genChunks(&chunks, cx, cz);
            //uccallagain = updateChunks(&chunks);
            //setcallagain = true;
        }
        /*
        if (!setcallagain && uccallagain) {
            uccallagain = updateChunks(&chunks);
        }
        */
        //genChunks(&chunks, cx, cz);
        updateChunks(&chunks);
        updateCam();
        float accuracy = 0.1;
        float lookatx = cos(rendinf.camrot.x * M_PI / 180.0) * sin(rendinf.camrot.y * M_PI / 180.0);
        float lookaty = sin(rendinf.camrot.x * M_PI / 180.0);
        float lookatz = (cos(rendinf.camrot.x * M_PI / 180.0) * cos(rendinf.camrot.y * M_PI / 180.0)) * -1;
        float blockx = 0, blocky = 0, blockz = 0;
        float lastblockx = 0, lastblocky = 0, lastblockz = 0;
        uint8_t blockid = 0, blockid2 = 0;
        int dist = (float)(5.0 / accuracy);
        for (int i = 1; i < dist; ++i) {
            blockid2 = blockid;
            lastblockx = blockx;
            lastblocky = blocky;
            lastblockz = blockz;
            float depth = (float)i * accuracy;
            blockx = lookatx * depth + rendinf.campos.x;
            blockx -= (blockx < 0) ? 1.0 : 0.0;
            blocky = lookaty * depth + rendinf.campos.y;
            blocky -= (blocky < 0) ? 1.0 : 0.0;
            blockz = lookatz * depth + rendinf.campos.z;
            blockz += (blockz > 0) ? 1.0 : 0.0;
            struct blockdata bdata = getBlock(&chunks, 0, 0, 0, blockx, blocky, blockz);
            blockid = bdata.id;
            if (bdata.id && bdata.id != 7) break;
        }
        /*
        printf("info:\n[camx:%f][camy:%f][camz:%f]\n[x:%f][y:%f][z:%f]\n[%f][%f][%f]\n[%f][%f][%f]\n[%d]\n",
            rendinf.campos.x, rendinf.campos.y, rendinf.campos.z, lookatx, lookaty, lookatz, blockx, blocky, blockz, round(blockx), blocky, round(blockz), blockid);
        */
        static bool placehold = false;
        static bool destroyhold = false;
        if (!destroyhold && input.multi_actions & INPUT_GETMAFLAG(INPUT_ACTION_MULTI_PLACE)) {
            if (!placehold || (altutime() - ptime) >= 500000)
                if ((altutime() - ptime2) >= 125000 && blockid && blockid != 7 && (!blockid2 || blockid2 == 7)) {
                    ptime2 = altutime();
                    setBlock(&chunks, 0, 0, 0, lastblockx, lastblocky, lastblockz, (struct blockdata){1, 0, 0, 0});
                }
            placehold = true;
        } else {
            placehold = false;
            ptime = altutime();
        }
        if (!placehold && input.multi_actions & INPUT_GETMAFLAG(INPUT_ACTION_MULTI_DESTROY)) {
            if (!destroyhold || (altutime() - dtime) >= 500000)
                if ((altutime() - dtime2) >= 125000 && blockid && blockid != 7 && (!blockid2 || blockid2 == 7)) {
                    dtime2 = altutime();
                    setBlock(&chunks, 0, 0, 0, blockx, blocky, blockz, (struct blockdata){0, 0, 0, 0});
                }
            destroyhold = true;
        } else {
            destroyhold = false;
            dtime = altutime();
        }
        //printf("[%f]\n", rendinf.camrot.y);
        //putchar('\n');
        /*
        for (uint32_t i = 0; i < 48; ++i) {
            for (uint32_t j = 0; j < 48; ++j) {
                double s = perlin2d((double)(j + (int)noff) / 10, (double)(i + (int)noff) / 10, 1.0, 1);
                double s3 = s * 4;
                double s2 = perlin2d(((double)(-j - 1 - (int)noff) / 320), ((double)(-i - 1 - (int)noff) / 320), 1.0, 1);
                s2 *= 3;
                s *= 4 * s2;
                //printf("[%f]\n", s);
                renderModelAt(((int)s3 > 1) ? m1 : m2, (coord_3d){0.0 + (float)j, -1.0 + (int)s, 0.0 + (float)i}, false);
            }
        }
        */
        //putchar('\n');
        if (!rendinf.vsync || !rendinf.fps || fpsstarttime2 >= 1000000 / rendinf.fps) {
            renderChunks(&chunks);
            updateScreen();
            fpsstarttime2 = altutime();
        }
        #ifdef SHOWFPS
        ++fpsct;
        uint64_t curtime = altutime();
        if (curtime - fpsstarttime >= 1000000) {
            printf("rendered [%d] frames in %f seconds\n", fpsct, (float)(curtime - fpsstarttime) / 1000000.0);
            fpsstarttime = curtime;
            fpsct = 0;
        }
        #endif
        fpsmult = (float)(altutime() - starttime) / (1000000.0f / 60.0f);
        pmult = posmult * fpsmult;
        rmult = rotmult * fpsmult;
    }
    for (int i = 0; i < 16; ++i) {
        free(tmpbuf[i]);
    }
    free(tmpbuf);
}
