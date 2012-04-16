//
//  ofxOpenMesh.h
//  ofxOpenMesh
//
//  Created by Ali Nakipoglu on 4/10/12.
//  Copyright (c) 2012 Fiction. All rights reserved.
//

#pragma once

#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Tools/Utils/StripifierT.hh>

#include "ofMain.h"

// types

struct ofxOpenMeshDefaultTraits: public OpenMesh::DefaultTraits
{
    typedef OpenMesh::Vec3f     Point;
    typedef OpenMesh::Vec3f     Normal;
    typedef OpenMesh::Vec4uc    Color;
};

// Attribute Bits

enum ofxOpenMeshAttributeBits
{
    
    OFXOPENMESH_VERTICES    = 2,
    OFXOPENMESH_INDICES     = 4,
    OFXOPENMESH_NORMALS     = 8,
    OFXOPENMESH_COLORS      = 16,
    OFXOPENMESH_TEXCOORDS   = 32,
    OFXOPENMESH_ALL_ATTRIBS = 2 | 4 | 8 | 16 | 32
    
};

// OpenMesh to ofMesh Options

enum ofxOpenMeshToOfMeshOpt
{
    OFXOPENMESH_NONE                    = 0,
    OFXOPENMESH_USE_A_TRIANGULATED_COPY = 2,
    OFXOPENMESH_USE_STRIPIFY_INDICES    = 4,
};

// Function prototypes

template <typename InMeshT, typename OutMeshT>
inline void ofxOpenMeshAddVertices( const InMeshT &mesh, OutMeshT &outMesh );

template <typename InMeshT, typename OutMeshT>
inline void ofxOpenMeshAddIndices( const InMeshT &mesh, OutMeshT &outMesh );

template <typename InMeshT, typename OutMeshT>
inline void ofxOpenMeshAddIndicesStripifed( const InMeshT &mesh, OutMeshT &outMesh );

template <typename InMeshT, typename OutMeshT>
inline void ofxOpenMeshAddNormals( const InMeshT &mesh, OutMeshT &outMesh );

template <typename InMeshT, typename OutMeshT>
inline void ofxOpenMeshAddColors( const InMeshT &mesh, OutMeshT &outMesh );

template <typename InMeshT, typename OutMeshT>
inline void ofxOpenMeshAddTexCoords( const InMeshT &mesh, OutMeshT &outMesh );

template <typename InMeshT, typename OutMeshT>
OutMeshT ofxOpenMeshToOFMeshT(  const InMeshT &mesh,
                                const int attribs = OFXOPENMESH_ALL_ATTRIBS,
                                const int opt = OFXOPENMESH_NONE );

template <typename InMeshT>
InMeshT ofxOpenMeshToOpenMeshT( const ofMesh &mesh );

// Implementations

template <typename InMeshT, typename OutMeshT>
void ofxOpenMeshAddVertices( const InMeshT &mesh, OutMeshT &outMesh )
{
    typedef typename InMeshT::ConstVertexIter       ConstVertexIterT;
    typedef typename InMeshT::Point                 PointT;
    
    for( ConstVertexIterT vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt )
    {
        PointT  point   = mesh.point( vIt.handle() );
        
        outMesh.addVertex( ofVec3f( static_cast<float>( point[0] ),
                                    static_cast<float>( point[1] ),
                                    static_cast<float>( point[2] )
                                  ) );
    }
}

template <typename InMeshT, typename OutMeshT>
void ofxOpenMeshAddIndices( const InMeshT &mesh, OutMeshT &outMesh )
{
    typedef typename InMeshT::ConstFaceIter         ConstFaceIterT;
    typedef typename InMeshT::ConstFaceVertexIter   ConstFaceVertexIterT;
    
    for( ConstFaceIterT fIt = mesh.faces_begin(); fIt != mesh.faces_end(); ++fIt )
    {
        for( ConstFaceVertexIterT fvIt = mesh.cfv_iter( fIt.handle() ); fvIt; ++fvIt )
        {
            outMesh.addIndex( static_cast<ofIndexType>( fvIt.handle().idx() ) );
        }
    }
}

template <typename InMeshT, typename OutMeshT>
void ofxOpenMeshAddIndicesStripifed( const InMeshT &mesh, OutMeshT &outMesh )
{
    InMeshT copy;
    
    copy.assign( mesh );
    
    OpenMesh::StripifierT<InMeshT> stripifier( copy );
    
    stripifier.stripify();
    
    if ( stripifier.is_valid() )
    {
        for ( typename OpenMesh::StripifierT<InMeshT>::StripsIterator sIt = stripifier.begin(); sIt != stripifier.end(); ++sIt )
        {
            for( typename OpenMesh::StripifierT<InMeshT>::IndexIterator siIt = sIt->begin(); siIt != sIt->end(); ++siIt )
            {
                outMesh.addIndex( static_cast<ofIndexType>( *siIt ) );
            }
        }
    }

}

template <typename InMeshT, typename OutMeshT>
void ofxOpenMeshAddNormals( const InMeshT &mesh, OutMeshT &outMesh )
{
    typedef typename InMeshT::ConstVertexIter       ConstVertexIterT;
    typedef typename InMeshT::Normal                NormalT;
    
    if ( mesh.has_vertex_normals() )
    {
        for( ConstVertexIterT vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt )
        {
            NormalT  normal = mesh.normal( vIt.handle() );
            
            outMesh.addNormal( ofVec3f( static_cast<float>( normal[0] ),
                                        static_cast<float>( normal[1] ),
                                        static_cast<float>( normal[2] )
                                      ) );
        }
    }
}

template <typename InMeshT, typename OutMeshT>
void ofxOpenMeshAddColors( const InMeshT &mesh, OutMeshT &outMesh )
{
    typedef typename InMeshT::ConstVertexIter       ConstVertexIterT;
    typedef typename InMeshT::Color                 ColorT;
    
    if( mesh.has_vertex_colors() )
    {
        OpenMesh::Vec4uc color( 255, 255, 255, 255 );

        for( ConstVertexIterT vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt )
        {
            color   = OpenMesh::color_cast<OpenMesh::Vec4uc, ColorT>( mesh.color( vIt ) );
            
            outMesh.addColor( ofColor( color[0], color[1], color[2], color[3] ) );
        }
    }
}

template <typename InMeshT, typename OutMeshT>
void ofxOpenMeshAddTexCoords( const InMeshT &mesh, OutMeshT &outMesh )
{
    typedef typename InMeshT::ConstVertexIter       ConstVertexIterT;
    typedef typename InMeshT::TexCoord2D            TexCoord2DT;
    
    if ( mesh.has_vertex_texcoords2D() )
    {
        for( ConstVertexIterT vIt = mesh.vertices_begin(); vIt != mesh.vertices_end(); ++vIt )
        {
            TexCoord2DT  coord  = mesh.texcoord2D( vIt.handle() );
            
            outMesh.addNormal( ofVec2f( static_cast<float>( coord[0] ),
                                        static_cast<float>( coord[1] )
                                       ) );
        }
    }
}

template <typename InMeshT, typename OutMeshT>
OutMeshT ofxOpenMeshToOFMeshT(  const InMeshT &mesh,
                                const int attribs,
                                const int opt )
{
    OutMeshT result;
    
    if ( ( attribs & OFXOPENMESH_VERTICES ) != 0 )
    {
        ofxOpenMeshAddVertices<InMeshT, OutMeshT>( mesh, result );
    }
    
    if ( ( attribs & OFXOPENMESH_INDICES ) != 0 )
    {
        if ( ( opt & OFXOPENMESH_USE_A_TRIANGULATED_COPY ) != 0 )
        {
            InMeshT copy;
            
            copy.assign( mesh );
            copy.triangulate();
            
            if ( ( opt & OFXOPENMESH_USE_STRIPIFY_INDICES ) != 0 )
            {
                ofxOpenMeshAddIndicesStripifed<InMeshT, OutMeshT>( copy, result );
            } else {
                ofxOpenMeshAddIndices<InMeshT, OutMeshT>( copy, result );
            }
        } else {
            if ( ( opt & OFXOPENMESH_USE_STRIPIFY_INDICES ) != 0 )
            {
                ofxOpenMeshAddIndicesStripifed<InMeshT, OutMeshT>( mesh, result );
            } else {
                ofxOpenMeshAddIndices<InMeshT, OutMeshT>( mesh, result );
            }
        }
    }
    
    if ( ( attribs & OFXOPENMESH_NORMALS ) != 0 )
    {
        ofxOpenMeshAddNormals<InMeshT, OutMeshT>( mesh, result );
    }
    
    if ( ( attribs & OFXOPENMESH_COLORS ) != 0 )
    {
        ofxOpenMeshAddColors<InMeshT, OutMeshT>( mesh, result );
    }
    
    if ( ( attribs & OFXOPENMESH_TEXCOORDS ) != 0 )
    {
        ofxOpenMeshAddTexCoords<InMeshT, OutMeshT>( mesh, result );
    }
    
    return result;
}

template <typename InMeshT>
InMeshT ofxOpenMeshToOpenMeshT( const ofMesh &mesh )
{
    
}