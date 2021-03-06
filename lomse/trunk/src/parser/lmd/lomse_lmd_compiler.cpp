//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2014 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include "lomse_lmd_compiler.h"

#include <sstream>
#include "lomse_xml_parser.h"
#include "lomse_lmd_analyser.h"
#include "lomse_model_builder.h"
#include "lomse_injectors.h"
#include "lomse_internal_model.h"
#include "lomse_document.h"
#include "lomse_file_system.h"
#include "lomse_zip_stream.h"


using namespace std;

namespace lomse
{

//=======================================================================================
// LmdCompiler implementation
//=======================================================================================
LmdCompiler::LmdCompiler(XmlParser* p, LmdAnalyser* a, ModelBuilder* mb, Document* pDoc)
    : Compiler(p, a, mb, pDoc)
    , m_pXmlParser(p)
    , m_pLmdAnalyser(a)
{
}

//---------------------------------------------------------------------------------------
//constructor for testing: direct construction
LmdCompiler::LmdCompiler(LibraryScope& libraryScope, Document* pDoc)
    : Compiler()
{
    m_pXmlParser = Injector::inject_XmlParser(libraryScope, pDoc->get_scope());
    m_pLmdAnalyser = Injector::inject_LmdAnalyser(libraryScope, pDoc, m_pXmlParser);

    m_pParser = m_pXmlParser;
    m_pAnalyser = m_pLmdAnalyser;
    m_pModelBuilder = Injector::inject_ModelBuilder(pDoc->get_scope());
    m_pDoc = pDoc;
    m_fileLocator = "";
}

//---------------------------------------------------------------------------------------
LmdCompiler::~LmdCompiler()
{
}

//---------------------------------------------------------------------------------------
InternalModel* LmdCompiler::compile_file(const std::string& filename)
{
    m_fileLocator = filename;
    DocLocator locator(m_fileLocator);
    if (locator.get_inner_protocol() == DocLocator::k_zip)
    {
        InputStream* pFile = FileSystem::open_input_stream(m_fileLocator);
        ZipInputStream* zip  = static_cast<ZipInputStream*>(pFile);

        unsigned char* buffer = zip->get_as_string();
        m_pXmlParser->parse_cstring( (char *)buffer );

        delete pFile;
        delete buffer;
    }
    else //k_file
        m_pParser->parse_file(filename);

    XmlNode* root = m_pXmlParser->get_tree_root();
    if (root)
        return compile_parsed_tree(root);
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
InternalModel* LmdCompiler::compile_string(const std::string& source)
{
    m_fileLocator = "string:";
    m_pXmlParser->parse_text(source);
    return compile_parsed_tree( m_pXmlParser->get_tree_root() );
}

////---------------------------------------------------------------------------------------
//InternalModel* LmdCompiler::compile_input(LdpReader& reader)
//{
//    m_fileLocator = reader.get_locator();
//    m_pFinalTree = m_pParser->parse_input(reader);
//    return compile_parsed_tree(m_pFinalTree);
//}

//---------------------------------------------------------------------------------------
InternalModel* LmdCompiler::create_empty()
{
    m_pParser->parse_text("<lenmusdoc vers='0.0'><content/></lenmusdoc>");
    return compile_parsed_tree( m_pXmlParser->get_tree_root() );
}

//---------------------------------------------------------------------------------------
InternalModel* LmdCompiler::create_with_empty_score()
{
//    m_pFinalTree = m_pParser->parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6)(instrument (musicData)))))");
//    return compile_parsed_tree(m_pFinalTree);
    return NULL;    //TODO: Probably this method is not needed
}

//---------------------------------------------------------------------------------------
InternalModel* LmdCompiler::compile_parsed_tree(XmlNode* root)
{
    InternalModel* pIModel = m_pLmdAnalyser->analyse_tree(root, m_fileLocator);
    if (pIModel)
        m_pModelBuilder->build_model(pIModel);
    return pIModel;
}

////---------------------------------------------------------------------------------------
//SpLdpTree LmdCompiler::wrap_score_in_lenmusdoc(SpLdpTree pParseTree)
//{
//    SpLdpTree pFinalTree = parse_empty_doc();
//
//    LdpTree::depth_first_iterator it = pFinalTree->begin();
//    while (it != pFinalTree->end() && !(*it)->is_type(k_content))
//        ++it;
//    (*it)->append_child(pParseTree->get_root());
//
//    return pFinalTree;
//}
//
////---------------------------------------------------------------------------------------
//SpLdpTree LmdCompiler::parse_empty_doc()
//{
//    SpLdpTree pTree = m_pParser->parse_text("(lenmusdoc (vers 0.0) (content ))");
//    return pTree;
//}


}  //namespace lomse
