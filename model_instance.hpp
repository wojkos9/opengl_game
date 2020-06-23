#pragma once
#include "model.hpp"
#include "animate.hpp"
#include <queue>
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

enum AnimType {
    Start=0, Walk=1, Stop=2, Idle
};

class ModelInstance {

public:
    float t = 0;
    map<int, Bone> *bones;
    Model &ctx;
    vector<mat4> mbs;
    AnimatedModel &amodel;
    bool do_animation = true;
    bool working = true;
    AnimType curr_anim = Idle;
    int cycles = 1;
    int order[3] = {0, 2, 1};
    vec3 pos;
    float d_since = 0.f;
    float last_d = 0.f;
    queue<AnimType> animations;
    bool walking = false;
    
    float rot = 0.f;
    float target_rot = 0.f;
    bool rotating = false;
    float rot_speed = 1.f;
    bool clockwise = true;

    vec2 dir = vec2(0, 1);
    vec2 target = vec2(0);

    void stop() {
        if (walking) {
            animations.push(Stop);
            animations.push(Idle);
            cout << "stop" << endl;
            walking = false;
        }
        
    }

    void rotateTo(float rad) {
        cout << "ROT: " << rad << endl;
        target_rot = rad;
        rotating = true;
    }

    void update(float dt) {

        if (rotating) {
            rot += rot_speed * (2*clockwise-1) * dt;
            if (rot >= target_rot && clockwise || rot <= target_rot && (!clockwise)) {
                rot = target_rot;
                rotating = false;
            }
        } else {
            if (!working) {
                if (animations.size() > 0) {
                    curr_anim = animations.front();
                    last_d = 0;
                    animations.pop();
                    cout << "anim " << curr_anim << "/" << animations.size() << endl;
                }
            }
            
            // Animate bones
            if (curr_anim != Idle) {
                t += dt;
                working = animate(*bones, ctx, t, &d_since, order[curr_anim], 64);
                cout << d_since << endl;
                float dr = (d_since>last_d)?(d_since-last_d):d_since;
                
                dr = dr*0.005;
                //cout << dr << pos.z << endl;
                pos += rotateY(vec3(0, 0, dr), rot);
                if (distance(vec2(pos.x, pos.z), target) <= pow(0.5f, 2)) {
                    stop();
                }
                last_d = d_since;
            } else {
                working = false;
                t = 0;
                last_d = 0;
            }
        }
        
        

        // Compute bone matrices
        int i = 0;
        for(int j : ctx.skins[0].joints) {
            mbs[i] = compute_mn(&(*bones)[j]) * *(*bones)[j].ob;
            i++;
        }
    }

    void start() {
        if (!walking) {
            animations.push(Start);
            animations.push(Walk);
            cout << "start" << endl;
            walking = true;
        }
    }

    void goTo(vec2 dst) {
        vec2 p = vec2(pos.x, pos.z);
        target = dst;
        dir = normalize(dst-p);
        float new_rot = acos(dot(dir, vec2(0, 1)));
        rotateTo(new_rot);
        start();

    }

    
    ModelInstance(AnimatedModel &am) : amodel(am), ctx(am.ctx) {
        bones = copyBones(am.bones);
        prepareBones();
        pos = vec3(0, -0.5f, 0);
    }
    void prepareBones() {

        // CPU buffer for bone matrices
        int n_bones = bones->size();
        mbs = vector<mat4>(n_bones, mat4());
    }
    void draw(Renderer &r) {
       
        r.passBoneMatrices(mbs);
        r.renderModel(amodel);
    }
};