/*
 * Copyright (c) 1996-2002 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

/************************************************************************
 * Windows NT/2000 object files.
 * $Id: Object-nt.h,v 1.18 2002/12/20 07:49:56 jaw Exp $
************************************************************************/




#if !defined(_Object_nt_h_)
#define _Object_nt_h_



/************************************************************************
 * header files.
************************************************************************/

#include "common/h/Dictionary.h"
#include "common/h/String.h"
#include "common/h/Symbol.h"
#include "common/h/Types.h"
#include "common/h/Vector.h"
#include "dyninstAPI/src/CodeView.h"

#include <stdlib.h>
#include <winnt.h>
#ifdef mips_unknown_ce2_11
#include <imagehlp.h> //ccw 7 apr 2001
#endif





/************************************************************************
 * class Object
************************************************************************/

class Object : public AObject
{
public:
	Object(const string, void (*)(const char *) = log_msg);
	Object(const string, Address baseAddress = NULL,
			void (*)(const char *) = log_msg);
	Object(const Object&);
	// "Filedescriptor" ctor
	Object(fileDescriptor *desc, Address baseAddress = 0, void (*)(const char *) = log_msg);

	virtual ~Object( void );

	Object&   operator=(const Object &);

	bool isEEL() const { return false; }

#if defined(mips_unknown_ce2_11) //ccw 28 mar 2001
	Address get_base_addr() const { return baseAddr;} //ccw 20 july 2000
	bool set_gp_value(Address addr) {  gp_value = addr; return true;} //ccw 27 july 2000
	Address get_gp_value() const { return gp_value;} //ccw 20 july 2000
#endif

private:
    // struct defining information we keep about a Paradyn module
    struct PDModInfo
    {
        string          name;       // name of Paradyn module
        unsigned int    offText;    // offset of module's code in text segment
        unsigned int    cbText;     // size of module's code in text segment

        PDModInfo( void )
          : name( "" ),
            offText( 0 ),
            cbText( 0 )
        {}

        PDModInfo( string modName, unsigned int offset, unsigned int cb )
          : name( modName ),
            offText( offset ),
            cbText( cb )
        {}

        PDModInfo& operator=( const PDModInfo& mi )
        {
            if( &mi != this )
            {
                name = mi.name;
                offText = mi.offText;
                cbText = mi.cbText;
            }
            return *this;
        }
    };

    // struct defining information we keep about a CodeView module
    struct ModInfo
    {
	    const CodeView::Module*	pCVMod;     // CodeView information
        unsigned int pdModIdx;              // index of Paradyn module
                                            // this module is associated with
                                            // in the pdMods array

	    ModInfo( const CodeView::Module* pCVModule = NULL,
                 unsigned int pdModIndex = 0 )
	      : pCVMod( pCVModule ),
		    pdModIdx( pdModIndex )
        {}

        ModInfo& operator=( const ModInfo& mi )
        {
            if( &mi != this )
            {
                pCVMod = mi.pCVMod;
                pdModIdx = mi.pdModIdx;
            }
            return *this;
        }

        static int CompareByOffset( const void* x, const void* y );
    };

	static	string	FindName(const char* stringTable, const IMAGE_SYMBOL& sym);
    static  string  FindModuleByOffset( unsigned int offset,
                                        const pdvector<PDModInfo>& pdMods );

    void    ParseDebugInfo( void );
	void	ParseSectionMap( IMAGE_DEBUG_INFORMATION* pDebugInfo );
	void	ParseCOFFSymbols( IMAGE_DEBUG_INFORMATION* pDebugInfo );
#ifdef mips_unknown_ce2_11 
	bool	ParseMapSymbols(IMAGE_DEBUG_INFORMATION *pDebugInfo, char *mapFile); //ccw 19 july 2000 : 28 mar 2001
#endif


#ifndef mips_unknown_ce2_11
	void	ParseExports( IMAGE_DEBUG_INFORMATION* pDebugInfo );
#endif // mips_unknown_ce2_11

    bool	ParseCodeViewSymbols( IMAGE_DEBUG_INFORMATION* pDebugInfo );
    void    CVPatchSymbolSizes( pdvector<Symbol>& allSymbols );
    void    CVProcessSymbols( CodeView& cv,
                              pdvector<ModInfo>& cvMods,
                              pdvector<PDModInfo>& pdMods,
                              pdvector<Symbol>& allSymbols );


	Address	baseAddr;					// location of this object in 
								// mutatee address space

#if defined(mips_unknown_ce2_11)
	Address gp_value;				//pointer to global area 
							//ccw 20 july 2000 : 28 mar 2001
#endif

	IMAGE_DEBUG_INFORMATION* pDebugInfo;	// debugging information 
						// mapped into our address space
	unsigned int textSectionId;		// id of .text segment (section)
	unsigned int dataSectionId;		// id of .data segment (section)
};


inline
Object::Object(const string file, 
				Address baseAddress,
				void (*err_func)(const char *))
  : AObject(file, err_func),
	baseAddr( baseAddress ),
	pDebugInfo( NULL )
{
	ParseDebugInfo();
}


inline
Object::Object(const string file, void (*err_func)(const char *))
  : AObject(file, err_func),
	baseAddr( 0 ),
	pDebugInfo( NULL )
{
	ParseDebugInfo();
}

inline
Object::Object(fileDescriptor *desc, Address baseAddress, void (*err_func)(const char *)) 
  : AObject(desc->file(), err_func),
    baseAddr(desc->addr()),
    pDebugInfo( NULL) 
{
	baseAddr = baseAddress;
  ParseDebugInfo();
}

inline
Object::Object(const Object& obj)
  : AObject(obj),
	baseAddr( obj.baseAddr ),
	pDebugInfo( obj.pDebugInfo )
{
	ParseDebugInfo();
}

inline
Object&
Object::operator=(const Object& obj)
{
	if( &obj != this )
	{
		AObject::operator=(obj);
		baseAddr = obj.baseAddr;
		pDebugInfo = obj.pDebugInfo;
	}
    return *this;
}

#endif /* !defined(_Object_nt_h_) */
