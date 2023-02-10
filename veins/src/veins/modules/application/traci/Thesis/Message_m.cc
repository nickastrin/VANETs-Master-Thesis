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

class DequeDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    DequeDescriptor();
    virtual ~DequeDescriptor();

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

Register_ClassDescriptor(DequeDescriptor)

DequeDescriptor::DequeDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Deque)), "")
{
    propertynames = nullptr;
}

DequeDescriptor::~DequeDescriptor()
{
    delete[] propertynames;
}

bool DequeDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Deque *>(obj)!=nullptr;
}

const char **DequeDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *DequeDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int DequeDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int DequeDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *DequeDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int DequeDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *DequeDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **DequeDescriptor::getFieldPropertyNames(int field) const
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

const char *DequeDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int DequeDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Deque *pp = (Deque *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *DequeDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Deque *pp = (Deque *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string DequeDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Deque *pp = (Deque *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool DequeDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    Deque *pp = (Deque *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *DequeDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *DequeDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Deque *pp = (Deque *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("MessageType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("MessageType"));
    e->insert(BROADCAST, "BROADCAST");
    e->insert(REQUEST, "REQUEST");
    e->insert(REPLY, "REPLY");
    e->insert(ROUTE_REQ, "ROUTE_REQ");
    e->insert(ROUTE_REPLY, "ROUTE_REPLY");
    e->insert(CENTRALITY_REQ, "CENTRALITY_REQ");
    e->insert(CENTRALITY_REPLY, "CENTRALITY_REPLY");
    e->insert(ACKNOWLEDGEMENT, "ACKNOWLEDGEMENT");
    e->insert(ORIGIN_INIT_REQ, "ORIGIN_INIT_REQ");
    e->insert(ORIGIN_INIT_REPLY, "ORIGIN_INIT_REPLY");
    e->insert(RSU_INIT_REQ, "RSU_INIT_REQ");
    e->insert(RSU_INIT_REPLY, "RSU_INIT_REPLY");
    e->insert(PULL_REQ, "PULL_REQ");
    e->insert(PULL_REPLY, "PULL_REPLY");
    e->insert(PUSH_CENTRALITY, "PUSH_CENTRALITY");
    e->insert(PUSH_ML, "PUSH_ML");
    e->insert(PUSH_CONTENT, "PUSH_CONTENT");
    e->insert(ORIGIN_CENTRALITY_REQ, "ORIGIN_CENTRALITY_REQ");
    e->insert(ORIGIN_CENTRALITY_REPLY, "ORIGIN_CENTRALITY_REPLY");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("CurrentState");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("CurrentState"));
    e->insert(INITIALIZING, "INITIALIZING");
    e->insert(SENDING, "SENDING");
    e->insert(REQUESTING, "REQUESTING");
    e->insert(REPEATING, "REPEATING");
    e->insert(COLLECTING, "COLLECTING");
    e->insert(CACHING, "CACHING");
    e->insert(WRITING, "WRITING");
    e->insert(EXTRACTING, "EXTRACTING");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("CentralityType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("CentralityType"));
    e->insert(DEGREE, "DEGREE");
    e->insert(CLOSENESS, "CLOSENESS");
    e->insert(BETWEENNESS, "BETWEENNESS");
)

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
    this->msgId = other.msgId;
    this->senderAddress = other.senderAddress;
    this->recipient = other.recipient;
    this->source = other.source;
    this->dest = other.dest;
    this->senderPosition = other.senderPosition;
    this->type = other.type;
    this->state = other.state;
    this->centrality = other.centrality;
    this->originMessage = other.originMessage;
    this->updatePaths = other.updatePaths;
    this->hops = other.hops;
    this->maxHops = other.maxHops;
    this->msgInfo = other.msgInfo;
    this->ackInfo = other.ackInfo;
    this->route = other.route;
    this->previousNodes = other.previousNodes;
    this->contentId = other.contentId;
    this->content = other.content;
    this->segments = other.segments;
    this->segmentNumber = other.segmentNumber;
    this->multimedia = other.multimedia;
}

void Message::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->recipient);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->dest);
    doParsimPacking(b,this->senderPosition);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->state);
    doParsimPacking(b,this->centrality);
    doParsimPacking(b,this->originMessage);
    doParsimPacking(b,this->updatePaths);
    doParsimPacking(b,this->hops);
    doParsimPacking(b,this->maxHops);
    doParsimPacking(b,this->msgInfo);
    doParsimPacking(b,this->ackInfo);
    doParsimPacking(b,this->route);
    doParsimPacking(b,this->previousNodes);
    doParsimPacking(b,this->contentId);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->segments);
    doParsimPacking(b,this->segmentNumber);
    doParsimPacking(b,this->multimedia);
}

void Message::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->recipient);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->dest);
    doParsimUnpacking(b,this->senderPosition);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->state);
    doParsimUnpacking(b,this->centrality);
    doParsimUnpacking(b,this->originMessage);
    doParsimUnpacking(b,this->updatePaths);
    doParsimUnpacking(b,this->hops);
    doParsimUnpacking(b,this->maxHops);
    doParsimUnpacking(b,this->msgInfo);
    doParsimUnpacking(b,this->ackInfo);
    doParsimUnpacking(b,this->route);
    doParsimUnpacking(b,this->previousNodes);
    doParsimUnpacking(b,this->contentId);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->segments);
    doParsimUnpacking(b,this->segmentNumber);
    doParsimUnpacking(b,this->multimedia);
}

const LAddress::L2Type& Message::getMsgId() const
{
    return this->msgId;
}

void Message::setMsgId(const LAddress::L2Type& msgId)
{
    this->msgId = msgId;
}

const LAddress::L2Type& Message::getSenderAddress() const
{
    return this->senderAddress;
}

void Message::setSenderAddress(const LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

const LAddress::L2Type& Message::getRecipient() const
{
    return this->recipient;
}

void Message::setRecipient(const LAddress::L2Type& recipient)
{
    this->recipient = recipient;
}

const LAddress::L2Type& Message::getSource() const
{
    return this->source;
}

void Message::setSource(const LAddress::L2Type& source)
{
    this->source = source;
}

const LAddress::L2Type& Message::getDest() const
{
    return this->dest;
}

void Message::setDest(const LAddress::L2Type& dest)
{
    this->dest = dest;
}

const Coord& Message::getSenderPosition() const
{
    return this->senderPosition;
}

void Message::setSenderPosition(const Coord& senderPosition)
{
    this->senderPosition = senderPosition;
}

MessageType Message::getType() const
{
    return this->type;
}

void Message::setType(MessageType type)
{
    this->type = type;
}

CurrentState Message::getState() const
{
    return this->state;
}

void Message::setState(CurrentState state)
{
    this->state = state;
}

CentralityType Message::getCentrality() const
{
    return this->centrality;
}

void Message::setCentrality(CentralityType centrality)
{
    this->centrality = centrality;
}

bool Message::getOriginMessage() const
{
    return this->originMessage;
}

void Message::setOriginMessage(bool originMessage)
{
    this->originMessage = originMessage;
}

bool Message::getUpdatePaths() const
{
    return this->updatePaths;
}

void Message::setUpdatePaths(bool updatePaths)
{
    this->updatePaths = updatePaths;
}

int Message::getHops() const
{
    return this->hops;
}

void Message::setHops(int hops)
{
    this->hops = hops;
}

int Message::getMaxHops() const
{
    return this->maxHops;
}

void Message::setMaxHops(int maxHops)
{
    this->maxHops = maxHops;
}

float Message::getMsgInfo() const
{
    return this->msgInfo;
}

void Message::setMsgInfo(float msgInfo)
{
    this->msgInfo = msgInfo;
}

omnetpp::simtime_t Message::getAckInfo() const
{
    return this->ackInfo;
}

void Message::setAckInfo(omnetpp::simtime_t ackInfo)
{
    this->ackInfo = ackInfo;
}

const Deque& Message::getRoute() const
{
    return this->route;
}

void Message::setRoute(const Deque& route)
{
    this->route = route;
}

const Deque& Message::getPreviousNodes() const
{
    return this->previousNodes;
}

void Message::setPreviousNodes(const Deque& previousNodes)
{
    this->previousNodes = previousNodes;
}

const char * Message::getContentId() const
{
    return this->contentId.c_str();
}

void Message::setContentId(const char * contentId)
{
    this->contentId = contentId;
}

const char * Message::getContent() const
{
    return this->content.c_str();
}

void Message::setContent(const char * content)
{
    this->content = content;
}

int Message::getSegments() const
{
    return this->segments;
}

void Message::setSegments(int segments)
{
    this->segments = segments;
}

int Message::getSegmentNumber() const
{
    return this->segmentNumber;
}

void Message::setSegmentNumber(int segmentNumber)
{
    this->segmentNumber = segmentNumber;
}

bool Message::getMultimedia() const
{
    return this->multimedia;
}

void Message::setMultimedia(bool multimedia)
{
    this->multimedia = multimedia;
}

class MessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_msgId,
        FIELD_senderAddress,
        FIELD_recipient,
        FIELD_source,
        FIELD_dest,
        FIELD_senderPosition,
        FIELD_type,
        FIELD_state,
        FIELD_centrality,
        FIELD_originMessage,
        FIELD_updatePaths,
        FIELD_hops,
        FIELD_maxHops,
        FIELD_msgInfo,
        FIELD_ackInfo,
        FIELD_route,
        FIELD_previousNodes,
        FIELD_contentId,
        FIELD_content,
        FIELD_segments,
        FIELD_segmentNumber,
        FIELD_multimedia,
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
    return basedesc ? 22+basedesc->getFieldCount() : 22;
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
        0,    // FIELD_msgId
        0,    // FIELD_senderAddress
        0,    // FIELD_recipient
        0,    // FIELD_source
        0,    // FIELD_dest
        0,    // FIELD_senderPosition
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_state
        FD_ISEDITABLE,    // FIELD_centrality
        FD_ISEDITABLE,    // FIELD_originMessage
        FD_ISEDITABLE,    // FIELD_updatePaths
        FD_ISEDITABLE,    // FIELD_hops
        FD_ISEDITABLE,    // FIELD_maxHops
        FD_ISEDITABLE,    // FIELD_msgInfo
        0,    // FIELD_ackInfo
        FD_ISCOMPOUND,    // FIELD_route
        FD_ISCOMPOUND,    // FIELD_previousNodes
        FD_ISEDITABLE,    // FIELD_contentId
        FD_ISEDITABLE,    // FIELD_content
        FD_ISEDITABLE,    // FIELD_segments
        FD_ISEDITABLE,    // FIELD_segmentNumber
        FD_ISEDITABLE,    // FIELD_multimedia
    };
    return (field >= 0 && field < 22) ? fieldTypeFlags[field] : 0;
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
        "msgId",
        "senderAddress",
        "recipient",
        "source",
        "dest",
        "senderPosition",
        "type",
        "state",
        "centrality",
        "originMessage",
        "updatePaths",
        "hops",
        "maxHops",
        "msgInfo",
        "ackInfo",
        "route",
        "previousNodes",
        "contentId",
        "content",
        "segments",
        "segmentNumber",
        "multimedia",
    };
    return (field >= 0 && field < 22) ? fieldNames[field] : nullptr;
}

int MessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgId") == 0) return base+0;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderAddress") == 0) return base+1;
    if (fieldName[0] == 'r' && strcmp(fieldName, "recipient") == 0) return base+2;
    if (fieldName[0] == 's' && strcmp(fieldName, "source") == 0) return base+3;
    if (fieldName[0] == 'd' && strcmp(fieldName, "dest") == 0) return base+4;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderPosition") == 0) return base+5;
    if (fieldName[0] == 't' && strcmp(fieldName, "type") == 0) return base+6;
    if (fieldName[0] == 's' && strcmp(fieldName, "state") == 0) return base+7;
    if (fieldName[0] == 'c' && strcmp(fieldName, "centrality") == 0) return base+8;
    if (fieldName[0] == 'o' && strcmp(fieldName, "originMessage") == 0) return base+9;
    if (fieldName[0] == 'u' && strcmp(fieldName, "updatePaths") == 0) return base+10;
    if (fieldName[0] == 'h' && strcmp(fieldName, "hops") == 0) return base+11;
    if (fieldName[0] == 'm' && strcmp(fieldName, "maxHops") == 0) return base+12;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgInfo") == 0) return base+13;
    if (fieldName[0] == 'a' && strcmp(fieldName, "ackInfo") == 0) return base+14;
    if (fieldName[0] == 'r' && strcmp(fieldName, "route") == 0) return base+15;
    if (fieldName[0] == 'p' && strcmp(fieldName, "previousNodes") == 0) return base+16;
    if (fieldName[0] == 'c' && strcmp(fieldName, "contentId") == 0) return base+17;
    if (fieldName[0] == 'c' && strcmp(fieldName, "content") == 0) return base+18;
    if (fieldName[0] == 's' && strcmp(fieldName, "segments") == 0) return base+19;
    if (fieldName[0] == 's' && strcmp(fieldName, "segmentNumber") == 0) return base+20;
    if (fieldName[0] == 'm' && strcmp(fieldName, "multimedia") == 0) return base+21;
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
        "veins::LAddress::L2Type",    // FIELD_msgId
        "veins::LAddress::L2Type",    // FIELD_senderAddress
        "veins::LAddress::L2Type",    // FIELD_recipient
        "veins::LAddress::L2Type",    // FIELD_source
        "veins::LAddress::L2Type",    // FIELD_dest
        "veins::Coord",    // FIELD_senderPosition
        "MessageType",    // FIELD_type
        "CurrentState",    // FIELD_state
        "CentralityType",    // FIELD_centrality
        "bool",    // FIELD_originMessage
        "bool",    // FIELD_updatePaths
        "int",    // FIELD_hops
        "int",    // FIELD_maxHops
        "float",    // FIELD_msgInfo
        "omnetpp::simtime_t",    // FIELD_ackInfo
        "Deque",    // FIELD_route
        "Deque",    // FIELD_previousNodes
        "string",    // FIELD_contentId
        "string",    // FIELD_content
        "int",    // FIELD_segments
        "int",    // FIELD_segmentNumber
        "bool",    // FIELD_multimedia
    };
    return (field >= 0 && field < 22) ? fieldTypeStrings[field] : nullptr;
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
            if (!strcmp(propertyname, "enum")) return "MessageType";
            return nullptr;
        case FIELD_state:
            if (!strcmp(propertyname, "enum")) return "CurrentState";
            return nullptr;
        case FIELD_centrality:
            if (!strcmp(propertyname, "enum")) return "CentralityType";
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
        case FIELD_msgId: {std::stringstream out; out << pp->getMsgId(); return out.str();}
        case FIELD_senderAddress: {std::stringstream out; out << pp->getSenderAddress(); return out.str();}
        case FIELD_recipient: {std::stringstream out; out << pp->getRecipient(); return out.str();}
        case FIELD_source: {std::stringstream out; out << pp->getSource(); return out.str();}
        case FIELD_dest: {std::stringstream out; out << pp->getDest(); return out.str();}
        case FIELD_senderPosition: {std::stringstream out; out << pp->getSenderPosition(); return out.str();}
        case FIELD_type: return enum2string(pp->getType(), "MessageType");
        case FIELD_state: return enum2string(pp->getState(), "CurrentState");
        case FIELD_centrality: return enum2string(pp->getCentrality(), "CentralityType");
        case FIELD_originMessage: return bool2string(pp->getOriginMessage());
        case FIELD_updatePaths: return bool2string(pp->getUpdatePaths());
        case FIELD_hops: return long2string(pp->getHops());
        case FIELD_maxHops: return long2string(pp->getMaxHops());
        case FIELD_msgInfo: return double2string(pp->getMsgInfo());
        case FIELD_ackInfo: return simtime2string(pp->getAckInfo());
        case FIELD_route: {std::stringstream out; out << pp->getRoute(); return out.str();}
        case FIELD_previousNodes: {std::stringstream out; out << pp->getPreviousNodes(); return out.str();}
        case FIELD_contentId: return oppstring2string(pp->getContentId());
        case FIELD_content: return oppstring2string(pp->getContent());
        case FIELD_segments: return long2string(pp->getSegments());
        case FIELD_segmentNumber: return long2string(pp->getSegmentNumber());
        case FIELD_multimedia: return bool2string(pp->getMultimedia());
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
        case FIELD_type: pp->setType((MessageType)string2enum(value, "MessageType")); return true;
        case FIELD_state: pp->setState((CurrentState)string2enum(value, "CurrentState")); return true;
        case FIELD_centrality: pp->setCentrality((CentralityType)string2enum(value, "CentralityType")); return true;
        case FIELD_originMessage: pp->setOriginMessage(string2bool(value)); return true;
        case FIELD_updatePaths: pp->setUpdatePaths(string2bool(value)); return true;
        case FIELD_hops: pp->setHops(string2long(value)); return true;
        case FIELD_maxHops: pp->setMaxHops(string2long(value)); return true;
        case FIELD_msgInfo: pp->setMsgInfo(string2double(value)); return true;
        case FIELD_contentId: pp->setContentId((value)); return true;
        case FIELD_content: pp->setContent((value)); return true;
        case FIELD_segments: pp->setSegments(string2long(value)); return true;
        case FIELD_segmentNumber: pp->setSegmentNumber(string2long(value)); return true;
        case FIELD_multimedia: pp->setMultimedia(string2bool(value)); return true;
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
        case FIELD_route: return omnetpp::opp_typename(typeid(Deque));
        case FIELD_previousNodes: return omnetpp::opp_typename(typeid(Deque));
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
        case FIELD_msgId: return toVoidPtr(&pp->getMsgId()); break;
        case FIELD_senderAddress: return toVoidPtr(&pp->getSenderAddress()); break;
        case FIELD_recipient: return toVoidPtr(&pp->getRecipient()); break;
        case FIELD_source: return toVoidPtr(&pp->getSource()); break;
        case FIELD_dest: return toVoidPtr(&pp->getDest()); break;
        case FIELD_senderPosition: return toVoidPtr(&pp->getSenderPosition()); break;
        case FIELD_route: return toVoidPtr(&pp->getRoute()); break;
        case FIELD_previousNodes: return toVoidPtr(&pp->getPreviousNodes()); break;
        default: return nullptr;
    }
}

} // namespace veins

