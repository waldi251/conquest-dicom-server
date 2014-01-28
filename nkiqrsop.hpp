#ifndef	_NKIQRSOP_H_
#	define	_NKIQRSOP_H_

/*
19990317	ljz	NKI-specific code
20001128	ljz	Fix: Crashes happened when more than one 'ServerChildThread' was
			active. m_pDCO was wrongly shared by all threads !!
20010429        mvh     Added GETADDO to allow optional read ahead withing calling program
20010502        mvh     Added extrabytes pointer to Read and RetrieveOn
20020415        mvh     ProcessDDO now returns status (to allow compression error check)
20020613	ljz	Added prototypes for DecompressNKI and CompressNKI
20021115	mvh	Added Generic retrieve classes
20030522	ljz	Added prototype of ComputeCRC
20030701        mvh     QualifyOn now also has compression parameter
20030702        mvh     added ExtendedPDU_Service
20030704        mvh     Changed ProcessDDO parameter to **DDO (for recompress)
20050118	mvh	replaced thread local storage under linux with variables in object
20050121	mvh	Changed filename to lower case
20050211	mvh	Removed need for thread local storage
20090209	mvh	Added QueryMoveScript callback
20091231	bcb	Added HAVE_LIBJPEG (version 6c!) and HAVE_JASPER for external library support (beta for now)
			Changed char* to const char* and cast time_t as int for gcc4.2 warnings
20100111	mvh	Merged
20100703	mvh	Merged some bcb OpenJPG changes
20100706	bcb	Added support for J2K and Jasper
20100721	mvh	Merged
20110118	mvh	Derived ExtendedPDU_Service from CheckedPDU_Service
20110118	mvh	Added lua_State to ExtendedPDU_Service
20110119	mvh	Added *VariableVRs and ThreadNum to ExtendedPDU_Service
20110122	mvh	Added ExtendedPDU_Service destructor
*/

#include "lua.hpp"

class	ExtendedPDU_Service	:
	public		CheckedPDU_Service
	{
		BOOL	AddTransferSyntaxs(PresentationContext &);	// override original function
		char	RequestedCompressionType[64];
		char	AcceptedCompressionType[64];
	public:
		BOOL	SetRequestedCompressionType(const char *type);
		char*	GetAcceptedCompressionType(UID uid);
		ExtendedPDU_Service (char *filename = NULL) : CheckedPDU_Service(filename) 
		{ L = NULL;
		  memset(VariableVRs, 0, sizeof(VariableVRs));
		  ThreadNum = 0;
		};
		~ExtendedPDU_Service () 
		{ if (L) lua_close(L);
		  if (VariableVRs[0]) delete VariableVRs[0];
		  if (VariableVRs[1]) delete VariableVRs[1];
		  if (VariableVRs[2]) delete VariableVRs[2];
		};

		// scripting state per association
		lua_State *L;
		VR *VariableVRs[3];
		int ThreadNum;
	};

class	StandardRetrieveNKI	:
	public	CMoveRQ,
	public	CMoveRSP
	{
	public:
		StandardRetrieveNKI();
		virtual	BOOL	GetUID (UID	&uid) { return (uGetUID(uid)); };
		virtual	BOOL	uGetUID ( UID &) = 0;
		virtual	BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO) = 0;
		virtual	BOOL	SearchOn ( DICOMDataObject	*,
					Array < DICOMDataObject *> *) = 0;
		virtual	BOOL	RetrieveOn (	DICOMDataObject *,
						DICOMDataObject **,
						StandardStorage	**,
						DICOMCommandObject	   *,
					        Array < DICOMDataObject *> *,
						void *) = 0;
		virtual	BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * ) = 0;
		virtual	BOOL	CallBack (	DICOMCommandObject	*,
						DICOMDataObject	* ) = 0;
		BOOL	Read (	ExtendedPDU_Service *, DICOMCommandObject *, void *ExtraBytes );
		BOOL	Write ( PDU_Service	*, DICOMDataObject	*, BYTE	*);
	};


class	PatientRootRetrieveNKI	:
			public	StandardRetrieveNKI
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };

		inline	BOOL	Read (	ExtendedPDU_Service	*PDU,
					DICOMCommandObject	*DCO,
					void 			*ExtraBytes )
			{ return ( StandardRetrieveNKI :: Read ( PDU, DCO, ExtraBytes ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieveNKI :: Write ( PDU,
						DDO, ACRNema )); };
	};


class	StudyRootRetrieveNKI	:
			public	StandardRetrieveNKI
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };
		
		inline	BOOL	Read (	ExtendedPDU_Service	*PDU,
					DICOMCommandObject	*DCO,
					void 			*ExtraBytes )
			{ return ( StandardRetrieveNKI :: Read ( PDU, DCO, ExtraBytes ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieveNKI :: Write ( PDU,
						DDO, ACRNema )); };
	};


class	PatientStudyOnlyRetrieveNKI	:
			public	StandardRetrieveNKI
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };
		
		inline	BOOL	Read (	ExtendedPDU_Service	*PDU,
					DICOMCommandObject	*DCO,
					void 			*ExtraBytes )
			{ return ( StandardRetrieveNKI :: Read ( PDU, DCO, ExtraBytes ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieveNKI :: Write ( PDU,
						DDO, ACRNema )); };
	};
						

class	PatientRootRetrieveGeneric	:
			public	StandardRetrieveNKI
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };

		inline	BOOL	Read (	ExtendedPDU_Service	*PDU,
					DICOMCommandObject	*DCO,
					void 			*ExtraBytes )
			{ return ( StandardRetrieveNKI :: Read ( PDU, DCO, ExtraBytes ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieveNKI :: Write ( PDU,
						DDO, ACRNema )); };
	};


class	StudyRootRetrieveGeneric	:
			public	StandardRetrieveNKI
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };
		
		inline	BOOL	Read (	ExtendedPDU_Service	*PDU,
					DICOMCommandObject	*DCO,
					void 			*ExtraBytes )
			{ return ( StandardRetrieveNKI :: Read ( PDU, DCO, ExtraBytes ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieveNKI :: Write ( PDU,
						DDO, ACRNema )); };
	};


class	PatientStudyOnlyRetrieveGeneric	:
			public	StandardRetrieveNKI
	{
	public:
		BOOL	GetUID ( UID & );
		BOOL	uGetUID ( UID &uid ) { return ( GetUID(uid) ); };
		
		inline	BOOL	Read (	ExtendedPDU_Service	*PDU,
					DICOMCommandObject	*DCO,
					void 			*ExtraBytes )
			{ return ( StandardRetrieveNKI :: Read ( PDU, DCO, ExtraBytes ) ); };

		BOOL	Write (	PDU_Service	*PDU,
						DICOMDataObject	*DDO,
						BYTE	*ACRNema)
			{ return ( StandardRetrieveNKI :: Write ( PDU,
						DDO, ACRNema )); };
	};

BOOL ProcessDDO(DICOMDataObject** pDDO, DICOMCommandObject* pDCO, ExtendedPDU_Service *PDU);
BOOL DecompressNKI(DICOMDataObject* pDDO);
BOOL CompressNKI(DICOMDataObject* pDDO, int iCompressMode = 2);

#ifdef HAVE_LIBJPEG
BOOL CompressJPEGL(DICOMDataObject* pDDO, int comp = '1', int jpegQuality = 95 );
BOOL DecompressJPEGL(DICOMDataObject* pDDO);
#endif //End HAVE_LIBJEPG

#ifdef HAVE_J2K // JPEG 2000 stuff
extern	int	DebugLevel;
#endif //End HAVE_J2K
#ifdef HAVE_LIBJASPER
BOOL CompressJPEG2K(DICOMDataObject* pDDO, int j2kQuality);
BOOL DecompressJPEG2K(DICOMDataObject* pDDO);
#endif //End LIBJASPER
#ifdef HAVE_LIBOPENJPEG
BOOL CompressJPEG2Ko(DICOMDataObject* pDDO, int j2kQuality);
BOOL DecompressJPEG2Ko(DICOMDataObject* pDDO);
#endif //End LIBOPENJPEG

unsigned int ComputeCRC(char* pcData, int iNbChars);
BOOL recompress(DICOMDataObject **pDDO, const char *Compression, const char *Filename, BOOL StripGroup2, ExtendedPDU_Service *PDU);
BOOL recompressFile(char *File, char *Compression, ExtendedPDU_Service *PDU);

#endif
