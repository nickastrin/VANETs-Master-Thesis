//
// Generated file, do not edit! Created by nedtool 5.6 from veins/modules/application/traci/Thesis/Message.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "Message_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

class ListDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    ListDescriptor();
    virtual ~ListDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ListDescriptor)

ListDescriptor::ListDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(List)), "")
{
    propertynames = nullptr;
}

ListDescriptor::~ListDescriptor()
{
    delete[] propertynames;
}

bool ListDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<List *>(obj)!=nullptr;
}

const char **ListDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ListDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ListDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int ListDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *ListDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int ListDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ListDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **ListDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ListDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ListDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    List *pp = (List *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ListDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    List *pp = (List *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ListDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    List *pp = (List *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool ListDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    List *pp = (List *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *ListDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *ListDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    List *pp = (List *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

namespace veins {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::messageType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::messageType"));
    e->insert(BROADCAST, "BROADCAST");
    e->insert(REQUEST, "REQUEST");
    e->insert(REPLY, "REPLY");
    e->insert(RSU_REQUEST, "RSU_REQUEST");
    e->insert(CENTRALITY_REQUEST, "CENTRALITY_REQUEST");
    e->insert(CENTRALITY_REPLY, "CENTRALITY_REPLY");
    e->insert(RSU_REPLY, "RSU_REPLY");
    e->insert(ACKNOWLEDGEMENT, "ACKNOWLEDGEMENT");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::currentState");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::currentState"));
    e->insert(INITIALIZING, "INITIALIZING");
    e->insert(SENDING, "SENDING");
    e->insert(COLLECTING, "COLLECTING");
    e->insert(CACHING, "CACHING");
    e->insert(TESTING, "TESTING");
    e->insert(REPEATING, "REPEATING");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::cachingPolicy");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::cachingPolicy"));
    e->insert(FIFO, "FIFO");
    e->insert(LRU, "LRU");
    e->insert(LFU, "LFU");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::scenario");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::scenario"));
    e->insert(CENTRALITY, "CENTRALITY");
    e->insert(CACHE, "CACHE");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::selectedCentrality");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::selectedCentrality"));
    e->insert(DEGREE, "DEGREE");
    e->insert(CLOSENESS, "CLOSENESS");
    e->insert(BETWEENNESS, "BETWEENNESS");
    e->insert(NONE, "NONE");
)

Register_Class(Message)

Message::Message(const char *name, short kind) : ::veins::BaseFrame1609_4(name, kind)
{
}

Message::Message(const Message& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

Message::~Message()
{
}

Message& Message::operator=(const Message& other)
{
    if (this == &other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void Message::copy(const Message& other)
{
    this->senderAddress = other.senderAddress;
    this->source = other.source;
    this->target = other.target;
    this->testFlag = other.testFlag;
    this->maxHops = other.maxHops;
    this->hops = other.hops;
    this->type = other.type;
    this->state = other.state;
    this->centrality = other.centrality;
    this->roadData = other.roadData;
    this->centralityData = other.centralityData;
    this->ackData = other.ackData;
    this->rsuList = other.rsuList;
    this->pathList = other.pathList;
    this->senderPosition = other.senderPosition;
}

void Message::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->target);
    doParsimPacking(b,this->testFlag);
    doParsimPacking(b,this->maxHops);
    doParsimPacking(b,this->hops);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->state);
    doParsimPacking(b,this->centrality);
    doParsimPacking(b,this->roadData);
    doParsimPacking(b,this->centralityData);
    doParsimPacking(b,this->ackData);
    doParsimPacking(b,this->rsuList);
    doParsimPacking(b,this->pathList);
    doParsimPacking(b,this->senderPosition);
}

void Message::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->target);
    doParsimUnpacking(b,this->testFlag);
    doParsimUnpacking(b,this->maxHops);
    doParsimUnpacking(b,this->hops);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->state);
    doParsimUnpacking(b,this->centrality);
    doParsimUnpacking(b,this->roadData);
    doParsimUnpacking(b,this->centralityData);
    doParsimUnpacking(b,this->ackData);
    doParsimUnpacking(b,this->rsuList);
    doParsimUnpacking(b,this->pathList);
    doParsimUnpacking(b,this->senderPosition);
}

const LAddress::L2Type& Message::getSenderAddress() const
{
    return this->senderAddress;
}

void Message::setSenderAddress(const LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

const LAddress::L2Type& Message::getSource() const
{
    return this->source;
}

void Message::setSource(const LAddress::L2Type& source)
{
    this->source = source;
}

const LAddress::L2Type& Message::getTarget() const
{
    return this->target;
}

void Message::setTarget(const LAddress::L2Type& target)
{
    this->target = target;
}

int Message::getTestFlag() const
{
    return this->testFlag;
}

void Message::setTestFlag(int testFlag)
{
    this->testFlag = testFlag;
}

int Message::getMaxHops() const
{
    return this->maxHops;
}

void Message::setMaxHops(int maxHops)
{
    this->maxHops = maxHops;
}

int Message::getHops() const
{
    return this->hops;
}

void Message::setHops(int hops)
{
    this->hops = hops;
}

veins::messageType Message::getType() const
{
    return this->type;
}

void Message::setType(veins::messageType type)
{
    this->type = type;
}

veins::currentState Message::getState() const
{
    return this->state;
}

void Message::setState(veins::currentState state)
{
    this->state = state;
}

veins::selectedCentrality Message::getCentrality() const
{
    return this->centrality;
}

void Message::setCentrality(veins::selectedCentrality centrality)
{
    this->centrality = centrality;
}

const char * Message::getRoadData() const
{
    return this->roadData.c_str();
}

void Message::setRoadData(const char * roadData)
{
    this->roadData = roadData;
}

int Message::getCentralityData() const
{
    return this->centralityData;
}

void Message::setCentralityData(int centralityData)
{
    this->centralityData = centralityData;
}

omnetpp::simtime_t Message::getAckData() const
{
    return this->ackData;
}

void Message::setAckData(omnetpp::simtime_t ackData)
{
    this->ackData = ackData;
}

const List& Message::getRsuList() const
{
    return this->rsuList;
}

void Message::setRsuList(const List& rsuList)
{
    this->rsuList = rsuList;
}

const List& Message::getPathList() const
{
    return this->pathList;
}

void Message::setPathList(const List& pathList)
{
    this->pathList = pathList;
}

const Coord& Message::getSenderPosition() const
{
    return this->senderPosition;
}

void Message::setSenderPosition(const Coord& senderPosition)
{
    this->senderPosition = senderPosition;
}

class MessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_senderAddress,
        FIELD_source,
        FIELD_target,
        FIELD_testFlag,
        FIELD_maxHops,
        FIELD_hops,
        FIELD_type,
        FIELD_state,
        FIELD_centrality,
        FIELD_roadData,
        FIELD_centralityData,
        FIELD_ackData,
        FIELD_rsuList,
        FIELD_pathList,
        FIELD_senderPosition,
    };
  public:
    MessageDescriptor();
    virtual ~MessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MessageDescriptor)

MessageDescriptor::MessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(veins::Message)), "veins::BaseFrame1609_4")
{
    propertynames = nullptr;
}

MessageDescriptor::~MessageDescriptor()
{
    delete[] propertynames;
}

bool MessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Message *>(obj)!=nullptr;
}

const char **MessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 15+basedesc->getFieldCount() : 15;
}

unsigned int MessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        0,    // FIELD_senderAddress
        0,    // FIELD_source
        0,    // FIELD_target
        FD_ISEDITABLE,    // FIELD_testFlag
        FD_ISEDITABLE,    // FIELD_maxHops
        FD_ISEDITABLE,    // FIELD_hops
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_state
        FD_ISEDITABLE,    // FIELD_centrality
        FD_ISEDITABLE,    // FIELD_roadData
        FD_ISEDITABLE,    // FIELD_centralityData
        0,    // FIELD_ackData
        FD_ISCOMPOUND,    // FIELD_rsuList
        FD_ISCOMPOUND,    // FIELD_pathList
        0,    // FIELD_senderPosition
    };
    return (field >= 0 && field < 15) ? fieldTypeFlags[field] : 0;
}

const char *MessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "senderAddress",
        "source",
        "target",
        "testFlag",
        "maxHops",
        "hops",
        "type",
        "state",
        "centrality",
        "roadData",
        "centralityData",
        "ackData",
        "rsuList",
        "pathList",
        "senderPosition",
    };
    return (field >= 0 && field < 15) ? fieldNames[field] : nullptr;
}

int MessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderAddress") == 0) return base+0;
    if (fieldName[0] == 's' && strcmp(fieldName, "source") == 0) return base+1;
    if (fieldName[0] == 't' && strcmp(fieldName, "target") == 0) return base+2;
    if (fieldName[0] == 't' && strcmp(fieldName, "testFlag") == 0) return base+3;
    if (fieldName[0] == 'm' && strcmp(fieldName, "maxHops") == 0) return base+4;
    if (fieldName[0] == 'h' && strcmp(fieldName, "hops") == 0) return base+5;
    if (fieldName[0] == 't' && strcmp(fieldName, "type") == 0) return base+6;
    if (fieldName[0] == 's' && strcmp(fieldName, "state") == 0) return base+7;
    if (fieldName[0] == 'c' && strcmp(fieldName, "centrality") == 0) return base+8;
    if (fieldName[0] == 'r' && strcmp(fieldName, "roadData") == 0) return base+9;
    if (fieldName[0] == 'c' && strcmp(fieldName, "centralityData") == 0) return base+10;
    if (fieldName[0] == 'a' && strcmp(fieldName, "ackData") == 0) return base+11;
    if (fieldName[0] == 'r' && strcmp(fieldName, "rsuList") == 0) return base+12;
    if (fieldName[0] == 'p' && strcmp(fieldName, "pathList") == 0) return base+13;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderPosition") == 0) return base+14;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "veins::LAddress::L2Type",    // FIELD_senderAddress
        "veins::LAddress::L2Type",    // FIELD_source
        "veins::LAddress::L2Type",    // FIELD_target
        "int",    // FIELD_testFlag
        "int",    // FIELD_maxHops
        "int",    // FIELD_hops
        "veins::messageType",    // FIELD_type
        "veins::currentState",    // FIELD_state
        "veins::selectedCentrality",    // FIELD_centrality
        "string",    // FIELD_roadData
        "int",    // FIELD_centralityData
        "omnetpp::simtime_t",    // FIELD_ackData
        "List",    // FIELD_rsuList
        "List",    // FIELD_pathList
        "veins::Coord",    // FIELD_senderPosition
    };
    return (field >= 0 && field < 15) ? fieldTypeStrings[field] : nullptr;
}

const char **MessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_type: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case FIELD_state: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case FIELD_centrality: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *MessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_type:
            if (!strcmp(propertyname, "enum")) return "veins::messageType";
            return nullptr;
        case FIELD_state:
            if (!strcmp(propertyname, "enum")) return "veins::currentState";
            return nullptr;
        case FIELD_centrality:
            if (!strcmp(propertyname, "enum")) return "veins::selectedCentrality";
            return nullptr;
        default: return nullptr;
    }
}

int MessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Message *pp = (Message *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Message *pp = (Message *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Message *pp = (Message *)object; (void)pp;
    switch (field) {
        case FIELD_senderAddress: {std::stringstream out; out << pp->getSenderAddress(); return out.str();}
        case FIELD_source: {std::stringstream out; out << pp->getSource(); return out.str();}
        case FIELD_target: {std::stringstream out; out << pp->getTarget(); return out.str();}
        case FIELD_testFlag: return long2string(pp->getTestFlag());
        case FIELD_maxHops: return long2string(pp->getMaxHops());
        case FIELD_hops: return long2string(pp->getHops());
        case FIELD_type: return enum2string(pp->getType(), "veins::messageType");
        case FIELD_state: return enum2string(pp->getState(), "veins::currentState");
        case FIELD_centrality: return enum2string(pp->getCentrality(), "veins::selectedCentrality");
        case FIELD_roadData: return oppstring2string(pp->getRoadData());
        case FIELD_centralityData: return long2string(pp->getCentralityData());
        case FIELD_ackData: return simtime2string(pp->getAckData());
        case FIELD_rsuList: {std::stringstream out; out << pp->getRsuList(); return out.str();}
        case FIELD_pathList: {std::stringstream out; out << pp->getPathList(); return out.str();}
        case FIELD_senderPosition: {std::stringstream out; out << pp->getSenderPosition(); return out.str();}
        default: return "";
    }
}

bool MessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    Message *pp = (Message *)object; (void)pp;
    switch (field) {
        case FIELD_testFlag: pp->setTestFlag(string2long(value)); return true;
        case FIELD_maxHops: pp->setMaxHops(string2long(value)); return true;
        case FIELD_hops: pp->setHops(string2long(value)); return true;
        case FIELD_type: pp->setType((veins::messageType)string2enum(value, "veins::messageType")); return true;
        case FIELD_state: pp->setState((veins::currentState)string2enum(value, "veins::currentState")); return true;
        case FIELD_centrality: pp->setCentrality((veins::selectedCentrality)string2enum(value, "veins::selectedCentrality")); return true;
        case FIELD_roadData: pp->setRoadData((value)); return true;
        case FIELD_centralityData: pp->setCentralityData(string2long(value)); return true;
        default: return false;
    }
}

const char *MessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_rsuList: return omnetpp::opp_typename(typeid(List));
        case FIELD_pathList: return omnetpp::opp_typename(typeid(List));
        default: return nullptr;
    };
}

void *MessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Message *pp = (Message *)object; (void)pp;
    switch (field) {
        case FIELD_senderAddress: return toVoidPtr(&pp->getSenderAddress()); break;
        case FIELD_source: return toVoidPtr(&pp->getSource()); break;
        case FIELD_target: return toVoidPtr(&pp->getTarget()); break;
        case FIELD_rsuList: return toVoidPtr(&pp->getRsuList()); break;
        case FIELD_pathList: return toVoidPtr(&pp->getPathList()); break;
        case FIELD_senderPosition: return toVoidPtr(&pp->getSenderPosition()); break;
        default: return nullptr;
    }
}

} // namespace veins

