

//
struct Vector3
{
public:
    float x;
    float y;
    float z;
public:
    Vector3() = default;
    Vector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
};

//
struct BiVector3
{
public:
    float x;
    float y;
    float z;
public:
    float length() const
    {
        return std::sqrtf(x*x + y*y + z*z);
    }
};

//
struct TriVector3
{
public:
    float x;
};

//
struct Vector4
{
public:
    float x;
    float y;
    float z;
    float w;
    //
    Vector4() = default;
    Vector4(float ax, float ay, float az, float aw)
    :x(ax),y(ay),z(az),w(aw)
    {}
    Vector4(float ax, float ay, float az)
    :x(ax),y(ay),z(az),w(1.0f)
    {}
};

//
struct BiVector4
{
public:
    float vx;
    float vy;
    float vz;
    float mx;
    float my;
    float mz;
public:
    Vector3 v() const
    {
        return Vector3(vx,vy,vz);
    }
    Vector3 m() const
    {
        return Vector3(mx,my,mz);
    }
};

//
struct TriVector4
{
public:
    float x;
    float y;
    float z;
    float w;
};

// wedge
BiVector3 join(Vector3 a, Vector3 b)
{
    return
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}
TriVector3 join(const BiVector3& ab, const Vector3& c)
{
    return
    {
        ab.x * c.x + ab.y * c.y + ab.z * c.z
    };
}
BiVector4 join(Vector4 a, Vector4 b)
{
    return
    {
        a.w * b.x - a.x * b.w,
        a.w * b.y - a.y * b.w,
        a.w * b.z - a.z * b.w,
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}
TriVector4 join(BiVector4 b, Vector4 p)
{
    return
    {
        b.vy * p.z - b.vz * p.y  + b.mx,
        b.vz * p.x - b.vx * p.z  + b.my,
        b.vx * p.x - b.vy * p.x  + b.mz,
        -b.mx * p.x - b.my * p.y - b.mz * p.z
    };
}

// antiwedge
BiVector3 meet(Vector3 a, Vector3 b)
{
    return
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}
BiVector4 meet(Vector4 a, Vector4 b)
{
    return
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        a.w * b.x - a.x * b.w,
        a.w * b.y - a.y * b.w,
        a.w * b.z - a.z * b.w,
    };
}
float meet(BiVector4 a, BiVector4 b)
{
    return
    -(a.vx * b.mx + a.vy * b.my + a.vz * b.mz)
    -(b.vx * a.mx + b.vy * a.my + b.vz * a.mz);
}
BiVector4 meet(TriVector4 a, TriVector4 b)
{
    return
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        a.w * b.x - a.x * b.w,
        a.w * b.y - a.y * b.w,
        a.w * b.z - a.z * b.w,
    };
}
Vector4 meet(BiVector4 a, TriVector4 b)
{
    return
    {
        +a.my * b.z - a.mz * b.y + a.vx * b.w,
        +a.mz * b.x - a.mx * b.z + a.vy * b.w,
        +a.mx * b.y - a.my * b.x + a.vz * b.w,
        -a.vx * b.x - a.vy * b.y - a.vz * b.z
    };
}

/*
 -------------------------------------------------
 main
 -------------------------------------------------
 */
int32_t main(int32_t argc, char* argv[])
{
    {
        // 二点から線を作る
        auto line = join(Vector4(1,1,0),Vector4(1,1,1));
        printf("Line Dir(%f,%f,%f) M(%f,%f,%f)\n", line.vx,line.vy,line.vz, line.mx,line.my,line.mz);
    }
    {
        // 二つの線の距離
        auto line0 = join(Vector4(0,0,0),Vector4(1,0,0));
        auto line1 = join(Vector4(0,0,1),Vector4(0,1,1));
        auto dist = meet(line0,line1) / join(line0.v(),line1.v()).length();
        printf("Dist:%f\n", dist);
    }
    {
        // 面と線の交点
        auto line = join(Vector4(1,0,0),Vector4(0,0,0));
        auto plane = join(join(Vector4(1,0,2),Vector4(0,1,2)),Vector4(0,0,2));
        auto point = meet(line,plane);
        printf("Pos:%f,%f,%f\n", point.x,point.y,point.z);
    }
    return 0;
}

