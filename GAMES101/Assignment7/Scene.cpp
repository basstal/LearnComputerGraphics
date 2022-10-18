//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <algorithm>

void Scene::buildBVH()
{
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum)
            {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
    const Ray &ray,
    const std::vector<Object *> &objects,
    float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear)
        {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection inter = intersect(ray);
    if (inter.happened)
    {
        if (inter.obj->hasEmit())
        {
            return Vector3f(1.0f);
        }
        return shade(inter, -ray.direction);
    }
    return Vector3f();
}

Vector3f Scene::shade(Intersection inInter, Vector3f wo) const
{
    Vector3f p = inInter.coords;
    Intersection inter = Intersection();
    float pdfLight = 0.0f;
    sampleLight(inter, pdfLight);
    Vector3f x = inter.coords;
    Vector3f ws = normalize(p - x);
    Vector3f emit = inter.emit;
    Ray x2p = Ray(x, ws);
    Intersection x2pInter = intersect(x2p);
    Vector3f n = inInter.normal;
    Vector3f nn = inter.normal;
    Vector3f lDir = Vector3f();
    Material *m = inInter.m;
    // std::cout << "p2xInter.happened :" << p2xInter.happened << std::endl;
    if (x2pInter.happened && x2pInter.obj == inInter.obj)
    {
        // std::cout <<  "lDir " << std::endl;
        lDir = emit * m->eval(-ws, wo, n) * dotProduct(-ws, n) * dotProduct(ws, nn) / pow((x - p).norm(), 2) / pdfLight;
    }

    Vector3f lIndir = Vector3f();
    float pRR = get_random_float();
    if (pRR < RussianRoulette)
    {
        Vector3f wi = normalize(m->sample(wo, n));
        Ray r = Ray(p, wi);
        Intersection rInter = intersect(r);
        if (rInter.happened && !rInter.obj->hasEmit())
        {
            lIndir = shade(rInter, -wi) * m->eval(wi, wo, n) * dotProduct(wi, n) / m->pdf(wi, wo, n) / RussianRoulette;
        }
    }

    return lDir + lIndir;
}