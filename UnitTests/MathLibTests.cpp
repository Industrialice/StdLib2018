#include "_PreHeader.hpp"
#include <MathFunctions.hpp>

using namespace StdLib;
using namespace Funcs;

void MathLibTests()
{
    Vector2 v0{0.5f, 0.75f};
    Vector2 v1{1.0f, 1.25f};
    Vector2 v2 = v0 + v1;
    UTest(true, EqualsWithEpsilon(v2.x, 1.5f) && EqualsWithEpsilon(v2.y, 2.0f));

    i32Vector2 iv0{1, 2};
    i32Vector2 iv1{3, 4};
    i32Vector2 iv2 = iv0 + iv1;
    UTest(true, iv2.x == 4 && iv2.y == 6);

    Vector3 rotTest(1.0f, 2.0f, 3.0f);
    auto rotMatrix = Matrix3x3::CreateRS(Vector3{1.0f, 2.0f, 3.0f});
    rotTest *= rotMatrix;
    rotMatrix.Transpose();
    rotTest *= rotMatrix;
    UTest(true, EqualsWithEpsilon(rotTest.x, 1.0f) && EqualsWithEpsilon(rotTest.y, 2.0f) && EqualsWithEpsilon(rotTest.z, 3.0f));

    PRINTLOG("finished math lib tests\n");
}