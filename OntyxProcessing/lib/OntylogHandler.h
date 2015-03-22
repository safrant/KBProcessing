#ifndef _ONTYX_HANDLER_H
#define _ONTYX_HANDLER_H

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/TransService.hpp>

#include <iostream>
#include <string>
#include <stack>

#include "OntyxKb.h"

using namespace std;
XERCES_CPP_NAMESPACE_USE

class OntylogHandler : public DefaultHandler {
private:
	string entityTag;
	string sdata; 

	string kindDefTag, namespaceTag, roleDefTag, propertyDefTag, conDefTag, qualifierDefTag, 
			parentsTag, roleTag, propertyTag, associationDefTag, associationTag, assocConTag,
			qualTag, pickTag, pickItemTag, rolegroupTag, infDefTag;

	bool isKind, isNamespace, isRoleDef, isPropertyDef, isConDef, isAssocDef, isQualifierDef, isPickList, hasRolegroup;
	string conname;
	string l_name;
	string l_code;
	string l_namespace;
	string l_domain;
	string l_range;
	string l_rightid;
	string l_parentrole;
	string l_mod;
	string l_value;
	string l_locale;
	string l_inverse;
	string l_type; 
	string q_name;
	string q_value;
	long l_id;
	bool l_display;
	bool l_reference;
	bool l_localized;
	bool l_indexed;
	bool localPrimitive;

	vector<string> s_pickList;
	vector<string> v_parents;
	vector<OntyxRole> v_roles;
	vector<OntyxProperty> v_properties;
	vector<OntyxAssociation> v_associations; // new
	vector<OntyxQualifier> v_qualifiers; // new
	stack<string> mStack;
	stack<string> entityStack;
	long rolegroupCounter;
	OntyxKb * tmpKb;

public:
	OntylogHandler(OntyxKb & inKb);

	virtual ~OntylogHandler();

	void initKindDef( void );
	void initNamespaceDef( void );
	void initRoleDef( void );
	void initPropertyDef( void );
	void initConDef( void );
	void initAssocDef( void );
	void initQualifierDef( void ); // new

	void constructKindDef( void );
	void constructNamespaceDef( void );
	void constructRoleDef( void );
	void constructPropertyDef( void );
	void constructConDef( void );
	void constructAssociationDef( void );
	void constructQualifierDef( void );

	void characters( const XMLCh * const chars, const unsigned int length);

	void startElement( const XMLCh * const uri, const XMLCh * const localname, 
						const XMLCh * const qname, const  Attributes & attrs);

	void endElement( const XMLCh * const uri, const XMLCh * const localname, 
						const XMLCh * const qname);

	void fatalError(const SAXParseException & exception);
};

#endif
