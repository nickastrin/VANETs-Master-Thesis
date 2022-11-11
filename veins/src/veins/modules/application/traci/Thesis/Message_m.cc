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

class SearchFrontDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    SearchFrontDescriptor();
    virtual ~SearchFrontDescriptor();

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

Register_ClassDescriptor(SearchFrontDescriptor)

SearchFrontDescriptor::SearchFrontDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(SearchFront)), "")
{
    propertynames = nullptr;
}

SearchFrontDescriptor::~SearchFrontDescriptor()
{
    delete[] propertynames;
}

bool SearchFrontDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SearchFront *>(obj)!=nullptr;
}

const char **SearchFrontDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *SearchFrontDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int SearchFrontDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int SearchFrontDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *SearchFrontDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int SearchFrontDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *SearchFrontDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **SearchFrontDescriptor::getFieldPropertyNames(int field) const
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

const char *SearchFrontDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int SearchFrontDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    SearchFront *pp = (SearchFront *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *SearchFrontDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SearchFront *pp = (SearchFront *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SearchFrontDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SearchFront *pp = (SearchFront *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool SearchFrontDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    SearchFront *pp = (SearchFront *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *SearchFrontDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *SearchFrontDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    SearchFront *pp = (SearchFront *)object; (void)pp;
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
    e->insert(RSU_CHECK, "RSU_CHECK");
    e->insert(RSU_REPLY, "RSU_REPLY");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::centralityType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::centralityType"));
    e->insert(NONE, "NONE");
    e->insert(DEGREE, "DEGREE");
    e->insert(CLOSENESS, "CLOSENESS");
    e->insert(BETWEENNESS, "BETWEENNESS");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("veins::procedureState");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("veins::procedureState"));
    e->insert(INITIALIZING, "INITIALIZING");
    e->insert(SENDING, "SENDING");
    e->insert(COLLECTING, "COLLECTING");
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
    this->target = other.target;
    this->maxHops = other.maxHops;
    this->hopCount = other.hopCount;
    this->type = other.type;
    this->centrality = other.centrality;
    this->state = other.state;
    this->messageData = other.messageData;
    this->passedRsu = other.passedRsu;
    this->searchFront = other.searchFront;
    this->senderPosition = other.senderPosition;
}

void Message::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->target);
    doParsimPacking(b,this->maxHops);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->centrality);
    doParsimPacking(b,this->state);
    doParsimPacking(b,this->messageData);
    doParsimPacking(b,this->passedRsu);
    doParsimPacking(b,this->searchFront);
    doParsimPacking(b,this->senderPosition);
}

void Message::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->target);
    doParsimUnpacking(b,this->maxHops);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->centrality);
    doParsimUnpacking(b,this->state);
    doParsimUnpacking(b,this->messageData);
    doParsimUnpacking(b,this->passedRsu);
    doParsimUnpacking(b,this->searchFront);
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

const LAddress::L2Type& Message::getTarget() const
{
    return this->target;
}

void Message::setTarget(const LAddress::L2Type& target)
{
    this->target = target;
}

int Message::getMaxHops() const
{
    return this->maxHops;
}

void Message::setMaxHops(int maxHops)
{
    this->maxHops = maxHops;
}

int Message::getHopCount() const
{
    return this->hopCount;
}

void Message::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

veins::messageType Message::getType() const
{
    return this->type;
}

void Message::setType(veins::messageType type)
{
    this->type = type;
}

veins::centralityType Message::getCentrality() const
{
    return this->centrality;
}

void Message::setCentrality(veins::centralityType centrality)
{
    this->centrality = centrality;
}

veins::procedureState Message::getState() const
{
    return this->state;
}

void Message::setState(veins::procedureState state)
{
    this->state = state;
}

const char * Message::getMessageData() const
{
    return this->messageData.c_str();
}

void Message::setMessageData(const char * messageData)
{
    this->messageData = messageData;
}

bool Message::getPassedRsu() const
{
    return this->passedRsu;
}

void Message::setPassedRsu(bool passedRsu)
{
    this->passedRsu = passedRsu;
}

const SearchFront& Message::getSearchFront() const
{
    return this->searchFront;
}

void Message::setSearchFront(const SearchFront& searchFront)
{
    this->searchFront = searchFront;
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
        FIELD_target,
        FIELD_maxHops,
        FIELD_hopCount,
        FIELD_type,
        FIELD_centrality,
        FIELD_state,
        FIELD_messageData,
        FIELD_passedRsu,
        FIELD_searchFront,
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
    return basedesc ? 11+basedesc->getFieldCount() : 11;
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
        0,    // FIELD_target
        FD_ISEDITABLE,    // FIELD_maxHops
        FD_ISEDITABLE,    // FIELD_hopCount
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_centrality
        FD_ISEDITABLE,    // FIELD_state
        FD_ISEDITABLE,    // FIELD_messageData
        FD_ISEDITABLE,    // FIELD_passedRsu
        FD_ISCOMPOUND,    // FIELD_searchFront
        0,    // FIELD_senderPosition
    };
    return (field >= 0 && field < 11) ? fieldTypeFlags[field] : 0;
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
        "target",
        "maxHops",
        "hopCount",
        "type",
        "centrality",
        "state",
        "messageData",
        "passedRsu",
        "searchFront",
        "senderPosition",
    };
    return (field >= 0 && field < 11) ? fieldNames[field] : nullptr;
}

int MessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderAddress") == 0) return base+0;
    if (fieldName[0] == 't' && strcmp(fieldName, "target") == 0) return base+1;
    if (fieldName[0] == 'm' && strcmp(fieldName, "maxHops") == 0) return base+2;
    if (fieldName[0] == 'h' && strcmp(fieldName, "hopCount") == 0) return base+3;
    if (fieldName[0] == 't' && strcmp(fieldName, "type") == 0) return base+4;
    if (fieldName[0] == 'c' && strcmp(fieldName, "centrality") == 0) return base+5;
    if (fieldName[0] == 's' && strcmp(fieldName, "state") == 0) return base+6;
    if (fieldName[0] == 'm' && strcmp(fieldName, "messageData") == 0) return base+7;
    if (fieldName[0] == 'p' && strcmp(fieldName, "passedRsu") == 0) return base+8;
    if (fieldName[0] == 's' && strcmp(fieldName, "searchFront") == 0) return base+9;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderPosition") == 0) return base+10;
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
        "veins::LAddress::L2Type",    // FIELD_target
        "int",    // FIELD_maxHops
        "int",    // FIELD_hopCount
        "veins::messageType",    // FIELD_type
        "veins::centralityType",    // FIELD_centrality
        "veins::procedureState",    // FIELD_state
        "string",    // FIELD_messageData
        "bool",    // FIELD_passedRsu
        "SearchFront",    // FIELD_searchFront
        "veins::Coord",    // FIELD_senderPosition
    };
    return (field >= 0 && field < 11) ? fieldTypeStrings[field] : nullptr;
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
        case FIELD_centrality: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case FIELD_state: {
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
        case FIELD_centrality:
            if (!strcmp(propertyname, "enum")) return "veins::centralityType";
            return nullptr;
        case FIELD_state:
            if (!strcmp(propertyname, "enum")) return "veins::procedureState";
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
        case FIELD_target: {std::stringstream out; out << pp->getTarget(); return out.str();}
        case FIELD_maxHops: return long2string(pp->getMaxHops());
        case FIELD_hopCount: return long2string(pp->getHopCount());
        case FIELD_type: return enum2string(pp->getType(), "veins::messageType");
        case FIELD_centrality: return enum2string(pp->getCentrality(), "veins::centralityType");
        case FIELD_state: return enum2string(pp->getState(), "veins::procedureState");
        case FIELD_messageData: return oppstring2string(pp->getMessageData());
        case FIELD_passedRsu: return bool2string(pp->getPassedRsu());
        case FIELD_searchFront: {std::stringstream out; out << pp->getSearchFront(); return out.str();}
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
        case FIELD_maxHops: pp->setMaxHops(string2long(value)); return true;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); return true;
        case FIELD_type: pp->setType((veins::messageType)string2enum(value, "veins::messageType")); return true;
        case FIELD_centrality: pp->setCentrality((veins::centralityType)string2enum(value, "veins::centralityType")); return true;
        case FIELD_state: pp->setState((veins::procedureState)string2enum(value, "veins::procedureState")); return true;
        case FIELD_messageData: pp->setMessageData((value)); return true;
        case FIELD_passedRsu: pp->setPassedRsu(string2bool(value)); return true;
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
        case FIELD_searchFront: return omnetpp::opp_typename(typeid(SearchFront));
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
        case FIELD_target: return toVoidPtr(&pp->getTarget()); break;
        case FIELD_searchFront: return toVoidPtr(&pp->getSearchFront()); break;
        case FIELD_senderPosition: return toVoidPtr(&pp->getSenderPosition()); break;
        default: return nullptr;
    }
}

} // namespace veins

