#include <memory>
#include <vector>

typedef std::shared_ptr<class Material> MaterialPtr;
typedef std::shared_ptr<class Mesh> MeshPtr;
typedef std::shared_ptr<class ModelAssImp> ModelAssPtr;
typedef std::shared_ptr<class Hitable> HitablePtr;
typedef std::vector<HitablePtr> HitableList;
typedef std::vector<MaterialPtr> MaterialList;
typedef std::vector<MeshPtr> MeshList;

typedef HitableList::iterator HitableIter;
typedef HitableList::const_iterator HitableConstIter;

