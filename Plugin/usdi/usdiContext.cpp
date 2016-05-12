#include "pch.h"
#include "usdiInternal.h"
#include "usdiSchema.h"
#include "usdiXform.h"
#include "usdiMesh.h"
#include "usdiContext.h"

namespace usdi {


Context::Context()
{
}

Context::~Context()
{
    unload();
}

bool Context::valid() const
{
    return m_stage;
}

void Context::unload()
{
    m_stage = UsdStageRefPtr();
    m_schemas.clear();
}

void Context::create(const char *identifier)
{
    unload();

    m_stage = UsdStage::CreateNew(identifier);
}


void Context::constructTreeRecursive(Schema *parent, UsdPrim prim)
{
    Schema *node = createNode(parent, prim);
    if (node) {
        m_schemas.emplace_back(node);

        auto children = prim.GetChildren();
        for (auto c : children) {
            constructTreeRecursive(node, c);
        }
    }
}

Schema* Context::createNode(Schema *parent, UsdPrim prim)
{
    UsdGeomMesh mesh(prim);
    UsdGeomXform xf(prim);

    Schema *ret = nullptr;
    if (mesh) {
        ret = new Mesh(this, parent, mesh);
    }
    else if (xf) {
        ret = new Xform(this, parent, xf);
    }

    return ret;
}

bool Context::open(const char *path)
{
    unload();

    m_stage = UsdStage::Open(path);
    if (m_stage == UsdStageRefPtr()) {
        return false;
    }

    m_start_time = m_stage->GetStartTimeCode();
    m_end_time = m_stage->GetEndTimeCode();

    UsdPrim root_prim = m_prim_root.empty() ? m_stage->GetDefaultPrim() : m_stage->GetPrimAtPath(SdfPath(m_prim_root));
    if (!root_prim && !(m_prim_root.empty() || m_prim_root == "/")) {
        root_prim = m_stage->GetPseudoRoot();
    }

    // Check if prim exists.  Exit if not
    if (!root_prim.IsValid()) {
        unload();
        return false;
    }

    // Set the variants on the usdRootPrim
    for (auto it = m_variants.begin(); it != m_variants.end(); ++it) {
        root_prim.GetVariantSet(it->first).SetVariantSelection(it->second);
    }

    constructTreeRecursive(nullptr, root_prim);
}

bool Context::write(const char *path)
{
    // todo
    return false;
}

const ImportConfig& Context::getImportConfig() const                { return m_import_config; }
void                Context::setImportConfig(const ImportConfig& v) { m_import_config = v; }
const ExportConfig& Context::getExportConfig() const                { return m_export_config; }
void                Context::setExportConfig(const ExportConfig& v) { m_export_config = v; }


Schema* Context::getRootNode()
{
    return m_schemas.empty() ? nullptr : m_schemas.front().get();
}

} // namespace usdi