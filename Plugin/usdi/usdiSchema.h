#pragma once

namespace usdi {

class Schema
{
public:
    Schema(Context *ctx, Schema *parent);
    virtual ~Schema();

    const ImportConfig& getImportConfig() const;
    const ExportConfig& getExportConfig() const;

    Context*            getContext() const;
    int                 getID() const;
    Schema*             getParent() const;
    size_t              getNumChildren() const;
    Schema*             getChild(int i) const;

    const char*         getPath() const;
    const char*         getName() const;
    const char*         getTypeName() const;
    UsdPrim             getUSDPrim() const;
    UsdTyped            getUSDSchema() const;
    virtual UsdTyped&   getUSDSchema() = 0;

public:
    void setID(int id);
    void addChild(Schema *child);

private:
    typedef std::vector<Schema*> Children;
    Context     *m_ctx;
    Schema      *m_parent;
    Children    m_children;
    int         m_id = 0;
};

} // namespace usdi
