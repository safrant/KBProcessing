

#include "OntylogHandler.h"

OntylogHandler::OntylogHandler(OntyxKb & inKb) {
	tmpKb = &inKb;
	kindDefTag = "kindDef";
	namespaceTag = "namespaceDef";
	roleTag = "role";
	rolegroupTag = "roleGroup";
	roleDefTag = "roleDef";
	propertyTag = "property";
	propertyDefTag = "propertyDef";
	assocConTag = "associations";
	associationTag = "association";
	associationDefTag = "associationDef";
	qualifierDefTag = "qualifierDef";
	qualTag = "qualifier";
	conDefTag = "conceptDef";
	infDefTag = "conceptInf";
	parentsTag = "definingConcepts";
	pickTag = "pickList";
	pickItemTag = "pickListItem";
	entityTag = "";
	isNamespace = false;
	isRoleDef = false;
	isPropertyDef = false;
	isAssocDef = false;
	isConDef = false;
	isKind = false;
	isQualifierDef = false;
	isPickList = false;
	}

OntylogHandler::~OntylogHandler() {}

void OntylogHandler::initKindDef( void ) {
	entityTag = kindDefTag;
	isKind = true;
	l_name = "";
	l_code = "";
	l_id = 0;
	l_namespace = "";
	l_reference = false;
	}

void OntylogHandler::initNamespaceDef( void ) {
	entityTag = namespaceTag;
	isNamespace = true;
	l_name = "";
	l_code = "";
	l_id = 0;
	}

void OntylogHandler::initRoleDef( void ) {
	entityTag = roleDefTag;
	isRoleDef = true;
	l_name = "";
	l_code = "";
	l_id = 0;
	l_namespace = "";
	l_domain = "";
	l_range = "";
	l_rightid = "";
	l_parentrole = "";
	}

void OntylogHandler::initPropertyDef( void ) {
	entityTag = propertyDefTag;
	isPropertyDef = true;
	l_name = "";
	l_code = "";
	l_id = -1;
	l_namespace = "";
	l_range = "";
	l_localized = false;
	l_indexed = false;
	s_pickList.clear();
	}

void OntylogHandler::initQualifierDef( void ) {
	entityTag = qualifierDefTag;
	isQualifierDef = true;
	l_name = "";
	l_code = "";
	l_id = -1;
	l_namespace = "";
	l_type = "";
	s_pickList.clear();
	}

void OntylogHandler::initAssocDef( void ) {
	entityTag = associationDefTag;
	isAssocDef = true;
	l_name = "";
	l_code = "";
	l_id = -1;
	l_namespace = "";
	l_inverse = "";
	l_display = false;
	}

void OntylogHandler::initConDef( void ) {
	entityTag = conDefTag;
	isConDef = true;
	conname = "";
	l_name = "";
	l_value = "";
	q_name = "";
	q_value = "";
	l_mod = "";
	l_locale = "";
	l_code = "";
	l_id = -1;
	l_domain = "";
	l_namespace = "";
	hasRolegroup = false;

	v_parents.clear();
	v_roles.clear();
	v_properties.clear();
	v_associations.clear();
	v_qualifiers.clear();
	localPrimitive = true;
	rolegroupCounter = 0;
	}

void OntylogHandler::constructKindDef( void ) {
	OntyxKind tmpName(l_name, l_id, l_code, l_namespace, l_reference);
	tmpKb->addKind(tmpName);
	}

void OntylogHandler::constructNamespaceDef( void ) {
	OntyxName tmpName(l_name, l_id, l_code);
	tmpKb->addNamespace(tmpName);
	}

void OntylogHandler::constructRoleDef( void ) {
	OntyxRoleDef tmpName(l_name, l_code, l_id, l_namespace, l_domain, l_range, l_rightid, l_parentrole);
	tmpKb->addRoleDef(tmpName);
	}

void OntylogHandler::constructPropertyDef( void ) {
	OntyxPropertyDef tmpName(l_name, l_code, l_id, l_range, l_localized, l_indexed, l_namespace);
	tmpName.setPickList(s_pickList);
	tmpKb->addPropertyDef(tmpName);
	s_pickList.clear();
	}

void OntylogHandler::constructAssociationDef( void ) {
	OntyxAssociationDef tmpName(l_name, l_code, l_id, l_inverse, l_display, l_namespace);
	tmpKb->addAssociationDef(tmpName);
	}

void OntylogHandler::constructConDef( void ) {
	OntyxConcept tmpConcept(conname, l_code, l_id, l_namespace, l_domain, localPrimitive);
	tmpConcept.addParents(v_parents).addRoles(v_roles).addProperties(v_properties).addAssociations(v_associations);
	tmpKb->addConcept(tmpConcept);
	}

void OntylogHandler::constructQualifierDef( void ) {
	OntyxQualifierDef tmpQual ( l_name, l_code, l_id, l_namespace, l_type, s_pickList);
	tmpKb->addQualifierDef(tmpQual);
	s_pickList.clear();
	}

void OntylogHandler::characters( const XMLCh * const chars, const unsigned int length) {
	char * data = new char [length+1];
	XMLString::transcode(chars, data, length);
	data[length] = 0; 
	sdata = data;
	XMLString::release(&data);

	// conceptDef, "primitive" is read as an end-element
	if( isConDef && entityStack.top() == conDefTag ) {
		if( mStack.top() == "name" ) 
			conname = sdata;
		else if( mStack.top() == "code" ) 
			l_code = sdata;
		else if( mStack.top() == "id" ) 
			l_id = atoi(sdata.c_str()) ;
		else if( mStack.top() == "namespace" ) 
			l_namespace = sdata;
		else if( mStack.top() == "kind" ) 
			l_domain = sdata;
		}
	else if( isConDef && entityStack.top() == parentsTag ) {
		if( mStack.top() == "concept" ) 
			v_parents.push_back(sdata);
		}
	else if( isConDef && entityStack.top() == propertyTag ) {
		if( mStack.top() == "name" )
			l_name = sdata;
		else if( mStack.top() == "value" )
			l_value = sdata; 
		else if( mStack.top() == "locale" )
			l_locale = sdata;
		}
	else if( isConDef && entityStack.top() == roleTag ) {
		if( mStack.top() == "name" )
			l_name = sdata;
		else if( mStack.top() == "value" )
			l_value = sdata; 
		}
	else if( isConDef && entityStack.top() == associationTag ) {
		if( mStack.top() == "name" )
			l_name = sdata;
		else if( mStack.top() == "value" )
			l_value = sdata; 
		}
	else if( isConDef && entityStack.top() == qualTag ) {
		if( mStack.top() == "name" )
			q_name = sdata;
		else if( mStack.top() == "value" )
			q_value = sdata; 
		}

	// associationDef section, displayable is read as an end-element
	else if( isAssocDef ) {
		if( mStack.top() == "name" ) 
			l_name = sdata;
		else if( mStack.top() == "code" ) 
			l_code = sdata;
		else if( mStack.top() == "id" ) 
			l_id = atoi(sdata.c_str());
		else if( mStack.top() == "namespace" ) 
			l_namespace = sdata;
		else if( mStack.top() == "inverseName" ) 
			l_inverse = sdata;
		}

	// propertyDef section, containsIndex is read as an end-element
	else if( isPropertyDef ) {
		if( mStack.top() == "name" ) 
			l_name = sdata;
		else if( mStack.top() == "code" ) 
			l_code = sdata;
		else if( mStack.top() == "id" ) 
			l_id = atoi(sdata.c_str());
		else if( mStack.top() == "namespace" ) 
			l_namespace = sdata;
		else if( mStack.top() == "range" ) 
			l_range = sdata;
		else if( mStack.top() == "localized" ) 
			l_localized = true;
		else if( mStack.top() == "pickListItem" )
			s_pickList.push_back(sdata);
		}

	// qualifierDef section
	else if( isQualifierDef ) {
		if( mStack.top() == "name" ) 
			l_name = sdata;
		else if( mStack.top() == "code" ) 
			l_code = sdata;
		else if( mStack.top() == "id" ) 
			l_id = atoi(sdata.c_str());
		else if( mStack.top() == "namespace" ) 
			l_namespace = sdata;
		else if( mStack.top() == "pickListItem" ) 
			s_pickList.push_back(sdata);
		}
	// roleDef section
	else if( isRoleDef ) {
		if( mStack.top() == "name" ) 
			l_name = sdata;
		else if( mStack.top() == "code" ) 
			l_code = sdata;
		else if( mStack.top() == "id" ) 
			l_id = atoi(sdata.c_str());
		else if( mStack.top() == "namespace" ) 
			l_namespace = sdata;
		else if( mStack.top() == "domain" ) 
			l_domain = sdata;
		else if( mStack.top() == "range" ) 
			l_range = sdata;
		else if( mStack.top() == "rightId" ) 
			l_rightid = sdata;
		else if( mStack.top() == "parentRole" )
			l_parentrole = sdata;
		}

	// kindDef section
	else if( isKind ) {
		if( mStack.top() == "name" )
			l_name = sdata;
		else if( mStack.top() == "code" )
			l_code = sdata;
		else if( mStack.top() == "id" )
			l_id = atoi(sdata.c_str());
		else if( mStack.top() == "namespace" )
			l_namespace = sdata;
		}
	// namespaceDef section
	else if( isNamespace ) {
		if( mStack.top() == "name" )
			l_name = sdata;
		else if( mStack.top() == "code" )
			l_code = sdata;
		else if( mStack.top() == "id" )
			l_id = atoi(sdata.c_str());
		}
	}

void OntylogHandler::startElement( const XMLCh * const uri, const XMLCh * const localname, 
					const XMLCh * const qname, const  Attributes & attrs) {
	string message(XMLString::transcode(localname));

	mStack.push(message);
	// conceptDef
	if( message == conDefTag || message == infDefTag ) {
		initConDef();
		entityStack.push(conDefTag);
		}
	else if( message == parentsTag ) {
		entityStack.push(message);
		}
	else if( message == roleTag ) {
		entityStack.push(message);
		}
	else if( message == propertyTag ) {
		entityStack.push(message);
		}
	else if( message == associationTag ) {
		entityStack.push(message);
		}
	else if( message == qualTag ) {
		entityStack.push(message);
		}
	else if( message == rolegroupTag ) {
		hasRolegroup = true;
		rolegroupCounter++;
		}

	// associationdef
	else if( message == associationDefTag ) {
		initAssocDef();
		}
	// propertydef
	else if( message == propertyDefTag ) {
		initPropertyDef();
		}
	// roledef
	else if( message == roleDefTag ) {
		initRoleDef();
		}
	//namespace def
	else if( message == namespaceTag ) {
		initNamespaceDef();
		}
	// kindDef section
	else if( message == kindDefTag ) {
		initKindDef();
		XMLCh * ref = XMLString::transcode("reference");
		string mref(XMLString::transcode(attrs.getValue(ref)));
		XMLString::release(&ref);
		if( mref == "false" )
			l_reference = false;
		else
			l_reference = true;
		}
	// qualifierDef section
	else if( message == qualifierDefTag ) {
		initQualifierDef();
		XMLCh * ref = XMLString::transcode("type");
		string mtype(XMLString::transcode(attrs.getValue(ref)));
		XMLString::release(&ref);
		l_type = mtype;
		}
	}

void OntylogHandler::endElement( const XMLCh * const uri, const XMLCh * const localname, 
					const XMLCh * const qname) {
	string mess(XMLString::transcode(localname));

	mStack.pop();
	if( mess == conDefTag || mess == infDefTag ) {
		constructConDef();
		entityStack.pop();
		isConDef = false;
		}
	else if( mess == parentsTag ) 
		entityStack.pop();
	else if( isConDef && entityStack.top() == roleTag && ( mess == "some" || mess == "all" || mess == "poss") )
		l_mod = mess;
	else if( mess == roleTag ) {
		entityStack.pop();
		if( hasRolegroup ) {
			OntyxRole tmpRole(l_name, l_value, l_mod, rolegroupCounter);
			v_roles.push_back(tmpRole);
			}
		else {
			OntyxRole tmpRole(l_name, l_value, l_mod);
			v_roles.push_back(tmpRole);
			}
		l_name = l_value = l_mod = "";
		}
	else if( mess == rolegroupTag ) {
		hasRolegroup = false;
		}
	else if( mess == propertyTag ) {
		entityStack.pop();
		OntyxProperty tmpProperty(l_name, l_value, l_locale);
		if( v_qualifiers.size() > 0 )
			tmpProperty.setQualifier(v_qualifiers);
		v_properties.push_back(tmpProperty);
		l_name = l_value = l_locale = "";
		v_qualifiers.clear();
		}	
	else if( mess == associationTag ) {
		entityStack.pop();
		OntyxAssociation tmpAssociation(l_name, l_value, v_qualifiers);
		v_associations.push_back(tmpAssociation);
		l_name = l_value = "";
		v_qualifiers.clear();
		}	
	else if( mess == qualTag ) {
		entityStack.pop();
		OntyxQualifier tmpQualifier(q_name, q_value);
		v_qualifiers.push_back(tmpQualifier);
		q_name = q_value = "";
		}	
		
	else if( mess == associationDefTag ) {
		constructAssociationDef();
		isAssocDef = false;
		}
	else if( mess == propertyDefTag ) {
		constructPropertyDef();
		isPropertyDef = false;
		}
	else if( mess == roleDefTag ) {
		constructRoleDef();
		isRoleDef = false;
		}
	else if( mess == qualifierDefTag ) {
		constructQualifierDef();
		isQualifierDef = false;
		}

	else if( mess == kindDefTag ) {
		constructKindDef();
		isKind = false;
		}
	else if( mess == namespaceTag ) {
		constructNamespaceDef();
		isNamespace = false;
		}
	else if( entityTag == propertyDefTag && mess == "containsIndex" )  {
		l_indexed = true;
		}
	else if( entityTag == conDefTag && mess == "primitive" ) {
		localPrimitive = false;
		}
	else if( entityTag == associationDefTag && mess == "displayable" ) {
		l_display = true;
		}
	}

void OntylogHandler::fatalError(const SAXParseException & exception) {
	string message( XMLString::transcode(exception.getMessage()));
	cout << "Fatal Error: " << message
		<< " at line: " << exception.getLineNumber() << endl;
	}





