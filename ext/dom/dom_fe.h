/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The JONJ Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@jonj.tk>                          |
   |          Rob Richards <rrichards@jonj.tk>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */
#ifndef DOM_FE_H
#define DOM_FE_H

extern const zend_function_entry jonj_dom_domexception_class_functions[];
extern const zend_function_entry jonj_dom_domstringlist_class_functions[];
extern const zend_function_entry jonj_dom_namelist_class_functions[];
extern const zend_function_entry jonj_dom_domimplementationlist_class_functions[];
extern const zend_function_entry jonj_dom_domimplementationsource_class_functions[];
extern const zend_function_entry jonj_dom_domimplementation_class_functions[];
extern const zend_function_entry jonj_dom_documentfragment_class_functions[];
extern const zend_function_entry jonj_dom_document_class_functions[];
extern const zend_function_entry jonj_dom_node_class_functions[];
extern const zend_function_entry jonj_dom_nodelist_class_functions[];
extern const zend_function_entry jonj_dom_namednodemap_class_functions[];
extern const zend_function_entry jonj_dom_characterdata_class_functions[];
extern const zend_function_entry jonj_dom_attr_class_functions[];
extern const zend_function_entry jonj_dom_element_class_functions[];
extern const zend_function_entry jonj_dom_text_class_functions[];
extern const zend_function_entry jonj_dom_comment_class_functions[];
extern const zend_function_entry jonj_dom_typeinfo_class_functions[];
extern const zend_function_entry jonj_dom_userdatahandler_class_functions[];
extern const zend_function_entry jonj_dom_domerror_class_functions[];
extern const zend_function_entry jonj_dom_domerrorhandler_class_functions[];
extern const zend_function_entry jonj_dom_domlocator_class_functions[];
extern const zend_function_entry jonj_dom_domconfiguration_class_functions[];
extern const zend_function_entry jonj_dom_cdatasection_class_functions[];
extern const zend_function_entry jonj_dom_documenttype_class_functions[];
extern const zend_function_entry jonj_dom_notation_class_functions[];
extern const zend_function_entry jonj_dom_entity_class_functions[];
extern const zend_function_entry jonj_dom_entityreference_class_functions[];
extern const zend_function_entry jonj_dom_processinginstruction_class_functions[];
extern const zend_function_entry jonj_dom_string_extend_class_functions[];
extern const zend_function_entry jonj_dom_xpath_class_functions[];

/* domexception errors */
typedef enum {
/* JONJ_ERR is non-spec code for JONJ errors: */
	JONJ_ERR                        = 0,
	INDEX_SIZE_ERR                 = 1,
	DOMSTRING_SIZE_ERR             = 2,
	HIERARCHY_REQUEST_ERR          = 3,
	WRONG_DOCUMENT_ERR             = 4,
	INVALID_CHARACTER_ERR          = 5,
	NO_DATA_ALLOWED_ERR            = 6,
	NO_MODIFICATION_ALLOWED_ERR    = 7,
	NOT_FOUND_ERR                  = 8,
	NOT_SUPPORTED_ERR              = 9,
	INUSE_ATTRIBUTE_ERR            = 10,
/* Introduced in DOM Level 2: */
	INVALID_STATE_ERR              = 11,
/* Introduced in DOM Level 2: */
	SYNTAX_ERR                     = 12,
/* Introduced in DOM Level 2: */
	INVALID_MODIFICATION_ERR       = 13,
/* Introduced in DOM Level 2: */
	NAMESPACE_ERR                  = 14,
/* Introduced in DOM Level 2: */
	INVALID_ACCESS_ERR             = 15,
/* Introduced in DOM Level 3: */
	VALIDATION_ERR                 = 16
} dom_exception_code;

/* domstringlist methods */
JONJ_FUNCTION(dom_domstringlist_item);

/* domnamelist methods */
JONJ_FUNCTION(dom_namelist_get_name);
JONJ_FUNCTION(dom_namelist_get_namespace_uri);

/* domimplementationlist methods */
JONJ_FUNCTION(dom_domimplementationlist_item);

/* domimplementationsource methods */
JONJ_FUNCTION(dom_domimplementationsource_get_domimplementation);
JONJ_FUNCTION(dom_domimplementationsource_get_domimplementations);

/* domimplementation methods */
JONJ_METHOD(domimplementation, hasFeature);
JONJ_METHOD(domimplementation, createDocumentType);
JONJ_METHOD(domimplementation, createDocument);
JONJ_METHOD(domimplementation, getFeature);

/* domdocumentfragment methods */
JONJ_METHOD(domdocumentfragment, __construct);
JONJ_METHOD(domdocumentfragment, appendXML);

/* domdocument methods */
JONJ_FUNCTION(dom_document_create_element);
JONJ_FUNCTION(dom_document_create_document_fragment);
JONJ_FUNCTION(dom_document_create_text_node);
JONJ_FUNCTION(dom_document_create_comment);
JONJ_FUNCTION(dom_document_create_cdatasection);
JONJ_FUNCTION(dom_document_create_processing_instruction);
JONJ_FUNCTION(dom_document_create_attribute);
JONJ_FUNCTION(dom_document_create_entity_reference);
JONJ_FUNCTION(dom_document_get_elements_by_tag_name);
JONJ_FUNCTION(dom_document_import_node);
JONJ_FUNCTION(dom_document_create_element_ns);
JONJ_FUNCTION(dom_document_create_attribute_ns);
JONJ_FUNCTION(dom_document_get_elements_by_tag_name_ns);
JONJ_FUNCTION(dom_document_get_element_by_id);
JONJ_FUNCTION(dom_document_adopt_node);
JONJ_FUNCTION(dom_document_normalize_document);
JONJ_FUNCTION(dom_document_rename_node);
JONJ_METHOD(domdocument, __construct);
	/* convienience methods */
JONJ_METHOD(domdocument, load);
JONJ_FUNCTION(dom_document_save);
JONJ_METHOD(domdocument, loadXML);
JONJ_FUNCTION(dom_document_savexml);
JONJ_FUNCTION(dom_document_validate);
JONJ_FUNCTION(dom_document_xinclude);
JONJ_METHOD(domdocument, registerNodeClass);

#if defined(LIBXML_HTML_ENABLED)
JONJ_METHOD(domdocument, loadHTML);
JONJ_METHOD(domdocument, loadHTMLFile);
JONJ_FUNCTION(dom_document_save_html);
JONJ_FUNCTION(dom_document_save_html_file);
#endif  /* defined(LIBXML_HTML_ENABLED) */

#if defined(LIBXML_SCHEMAS_ENABLED)
JONJ_FUNCTION(dom_document_schema_validate_file);
JONJ_FUNCTION(dom_document_schema_validate_xml);
JONJ_FUNCTION(dom_document_relaxNG_validate_file);
JONJ_FUNCTION(dom_document_relaxNG_validate_xml);
#endif

/* domnode methods */
JONJ_FUNCTION(dom_node_insert_before);
JONJ_FUNCTION(dom_node_replace_child);
JONJ_FUNCTION(dom_node_remove_child);
JONJ_FUNCTION(dom_node_append_child);
JONJ_FUNCTION(dom_node_has_child_nodes);
JONJ_FUNCTION(dom_node_clone_node);
JONJ_FUNCTION(dom_node_normalize);
JONJ_FUNCTION(dom_node_is_supported);
JONJ_FUNCTION(dom_node_has_attributes);
JONJ_FUNCTION(dom_node_compare_document_position);
JONJ_FUNCTION(dom_node_is_same_node);
JONJ_FUNCTION(dom_node_lookup_prefix);
JONJ_FUNCTION(dom_node_is_default_namespace);
JONJ_FUNCTION(dom_node_lookup_namespace_uri);
JONJ_FUNCTION(dom_node_is_equal_node);
JONJ_FUNCTION(dom_node_get_feature);
JONJ_FUNCTION(dom_node_set_user_data);
JONJ_FUNCTION(dom_node_get_user_data);
JONJ_METHOD(domnode, C14N);
JONJ_METHOD(domnode, C14NFile);
JONJ_METHOD(domnode, getNodePath);
JONJ_METHOD(domnode, getLineNo);

/* domnodelist methods */
JONJ_FUNCTION(dom_nodelist_item);

/* domnamednodemap methods */
JONJ_FUNCTION(dom_namednodemap_get_named_item);
JONJ_FUNCTION(dom_namednodemap_set_named_item);
JONJ_FUNCTION(dom_namednodemap_remove_named_item);
JONJ_FUNCTION(dom_namednodemap_item);
JONJ_FUNCTION(dom_namednodemap_get_named_item_ns);
JONJ_FUNCTION(dom_namednodemap_set_named_item_ns);
JONJ_FUNCTION(dom_namednodemap_remove_named_item_ns);

/* domcharacterdata methods */
JONJ_FUNCTION(dom_characterdata_substring_data);
JONJ_FUNCTION(dom_characterdata_append_data);
JONJ_FUNCTION(dom_characterdata_insert_data);
JONJ_FUNCTION(dom_characterdata_delete_data);
JONJ_FUNCTION(dom_characterdata_replace_data);

/* domattr methods */
JONJ_FUNCTION(dom_attr_is_id);
JONJ_METHOD(domattr, __construct);

/* domelement methods */
JONJ_FUNCTION(dom_element_get_attribute);
JONJ_FUNCTION(dom_element_set_attribute);
JONJ_FUNCTION(dom_element_remove_attribute);
JONJ_FUNCTION(dom_element_get_attribute_node);
JONJ_FUNCTION(dom_element_set_attribute_node);
JONJ_FUNCTION(dom_element_remove_attribute_node);
JONJ_FUNCTION(dom_element_get_elements_by_tag_name);
JONJ_FUNCTION(dom_element_get_attribute_ns);
JONJ_FUNCTION(dom_element_set_attribute_ns);
JONJ_FUNCTION(dom_element_remove_attribute_ns);
JONJ_FUNCTION(dom_element_get_attribute_node_ns);
JONJ_FUNCTION(dom_element_set_attribute_node_ns);
JONJ_FUNCTION(dom_element_get_elements_by_tag_name_ns);
JONJ_FUNCTION(dom_element_has_attribute);
JONJ_FUNCTION(dom_element_has_attribute_ns);
JONJ_FUNCTION(dom_element_set_id_attribute);
JONJ_FUNCTION(dom_element_set_id_attribute_ns);
JONJ_FUNCTION(dom_element_set_id_attribute_node);
JONJ_METHOD(domelement, __construct);

/* domtext methods */
JONJ_FUNCTION(dom_text_split_text);
JONJ_FUNCTION(dom_text_is_whitespace_in_element_content);
JONJ_FUNCTION(dom_text_replace_whole_text);
JONJ_METHOD(domtext, __construct);

/* domcomment methods */
JONJ_METHOD(domcomment, __construct);

/* domtypeinfo methods */

/* domuserdatahandler methods */
JONJ_FUNCTION(dom_userdatahandler_handle);

/* domdomerror methods */

/* domerrorhandler methods */
JONJ_FUNCTION(dom_domerrorhandler_handle_error);

/* domlocator methods */

/* domconfiguration methods */
JONJ_FUNCTION(dom_domconfiguration_set_parameter);
JONJ_FUNCTION(dom_domconfiguration_get_parameter);
JONJ_FUNCTION(dom_domconfiguration_can_set_parameter);

/* domcdatasection methods */
JONJ_METHOD(domcdatasection, __construct);

/* domdocumenttype methods */

/* domnotation methods */

/* domentity methods */

/* domentityreference methods */
JONJ_METHOD(domentityreference, __construct);

/* domprocessinginstruction methods */
JONJ_METHOD(domprocessinginstruction, __construct);

/* string_extend methods */
JONJ_FUNCTION(dom_string_extend_find_offset16);
JONJ_FUNCTION(dom_string_extend_find_offset32);

#if defined(LIBXML_XPATH_ENABLED)
/* xpath methods */
JONJ_METHOD(domxpath, __construct);
JONJ_FUNCTION(dom_xpath_register_ns);
JONJ_FUNCTION(dom_xpath_query);
JONJ_FUNCTION(dom_xpath_evaluate);
JONJ_FUNCTION(dom_xpath_register_jonj_functions);
#endif

#endif /* DOM_FE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
