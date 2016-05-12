#include "pch.h"
#include "usdiInternal.h"
#include "usdiSchema.h"
#include "usdiXform.h"

namespace usdi {


Xform::Xform(Context *ctx, Schema *parent, const UsdGeomXformable& xf)
    : super(ctx, parent)
    , m_xf(xf)
{
    usdiLog("constructed %s\n", getPath());
}

Xform::~Xform()
{
    usdiLog("destructed %s\n", getPath());
}

UsdGeomXformable& Xform::getUSDType()
{
    return m_xf;
}

SchemaType Xform::getType() const
{
    return SchemaType::Xform;
}

void Xform::readSample(XformData& dst, Time t_)
{
    auto t = (const UsdTimeCode&)t_;
    dst.position = { 0.0f, 0.0f, 0.0f };
    dst.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    dst.scale = { 1.0f, 1.0f, 1.0f };

    if (m_read_ops.empty()) {
        bool reset_stack = false;
        m_read_ops = m_xf.GetOrderedXformOps(&reset_stack);
    }

    for (auto& op : m_read_ops) {
        switch (op.GetOpType()) {
        case UsdGeomXformOp::TypeTranslate:
        {
            op.GetAs((GfVec3f*)&dst.position, t);
            break;
        }
        case UsdGeomXformOp::TypeOrient:
        {
            op.GetAs((GfVec4f*)&dst.rotation, t);
            break;
        }
        case UsdGeomXformOp::TypeScale:
        {
            op.GetAs((GfVec3f*)&dst.scale, t);
            break;
        }

        case UsdGeomXformOp::TypeRotateZXY:
        {
            // todo
            break;
        }
        case UsdGeomXformOp::TypeTransform:
        {
            // todo
            break;
        }
        }
    }
}

void Xform::writeSample(const XformData& src, Time t_)
{
    auto t = (const UsdTimeCode&)t_;

    if (m_write_ops.empty()) {
        m_write_ops.push_back(m_xf.AddTranslateOp(UsdGeomXformOp::PrecisionFloat));
        m_write_ops.push_back(m_xf.AddOrientOp(UsdGeomXformOp::PrecisionFloat));
        m_write_ops.push_back(m_xf.AddScaleOp(UsdGeomXformOp::PrecisionFloat));
    }
    {
        m_write_ops[0].Set((const GfVec3f&)src.position);
        m_write_ops[1].Set((const GfVec4f&)src.rotation);
        m_write_ops[2].Set((const GfVec3f&)src.scale);
    }
}

} // namespace usdi