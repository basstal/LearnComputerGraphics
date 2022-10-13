#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object *> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

Bounds3 BVHAccel::calculateBoundsOfObjects(const std::vector<Object *> &objects)
{
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
    {
        bounds = Union(bounds, objects[i]->getBounds());
    }
    return bounds;
}
BVHBuildNode *BVHAccel::recursiveBuild(std::vector<Object *> objects)
{
    BVHBuildNode *node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds = calculateBoundsOfObjects(objects);
    if (objects.size() == 1)
    {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2)
    {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else
    {

        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim)
        {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
                      { return f1->getBounds().Centroid().x <
                               f2->getBounds().Centroid().x; });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
                      { return f1->getBounds().Centroid().y <
                               f2->getBounds().Centroid().y; });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
                      { return f1->getBounds().Centroid().z <
                               f2->getBounds().Centroid().z; });
            break;
        }

        std::vector<Object *> leftshapes;
        std::vector<Object *> rightshapes;
        if (splitMethod == SplitMethod::NAIVE)
        {

            auto beginning = objects.begin();
            auto middling = objects.begin() + (objects.size() / 2);
            auto ending = objects.end();

            leftshapes = std::vector<Object *>(beginning, middling);
            rightshapes = std::vector<Object *>(middling, ending);
        }
        else
        {
            double bounds_surface_area = bounds.SurfaceArea();
            int partition_at = -1;
            float min_weight = std::numeric_limits<float>::infinity();
            // do 10 times
            for (int i = 0; i < 10; i++)
            {
                int partition = (int)(get_random_float() * objects.size());
                // std::cout << partition << std::endl;
                auto leftshapes = std::vector<Object *>(objects.begin(), objects.begin() + partition);
                auto rightshapes = std::vector<Object *>(objects.begin() + partition, objects.end());
                Bounds3 leftbounds = calculateBoundsOfObjects(leftshapes);
                Bounds3 rightbounds = calculateBoundsOfObjects(rightshapes);
                float weight = leftbounds.SurfaceArea() / bounds_surface_area * leftshapes.size() + rightbounds.SurfaceArea() / bounds_surface_area * rightshapes.size();
                if (weight < min_weight)
                {
                    min_weight = weight;
                    partition_at = partition;
                }
            }

            leftshapes = std::vector<Object *>(objects.begin(), objects.begin() + partition_at);
            rightshapes = std::vector<Object *>(objects.begin() + partition_at, objects.end());
        }
        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray &ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode *node, const Ray &ray) const
{
    // TODO Traverse the BVH to find intersection
    Intersection result;
    if (node == nullptr)
    {
        return result;
    }
    if (node->left == nullptr && node->right == nullptr) // is leaf
    {
        return node->object->getIntersection(ray);
    }
    std::array<int, 3> dirIsNeg = {(int)(ray.direction.x > 0), (int)(ray.direction.y > 0), (int)(ray.direction.z > 0)};
    if (!node->bounds.IntersectP(ray, ray.direction_inv, dirIsNeg))
    {
        return result;
    }
    Intersection inter_left = getIntersection(node->left, ray);
    Intersection inter_right = getIntersection(node->right, ray);
    if (!inter_left.happened)
    {
        return inter_right;
    }
    if (!inter_right.happened)
    {
        return inter_left;
    }
    return inter_left.distance < inter_right.distance ? inter_left : inter_right;
}