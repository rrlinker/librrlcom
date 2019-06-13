// Common
X(Unknown                      , 0x0ULL                        )
X(OK                           , 0x0CULL << 56                 )
X(NotOK                        , 0x7CULL << 56                 )

// client <-> svcreqhandler
X(Version                      , 0x01ULL                       )
X(Identify                     , 0x1DULL                       )
X(LinkLibrary                  , 0x111BLL                      )

// client <-> svclinker
X(ResolveExternalSymbol        , 0x435014332417BLL             )
X(ResolvedSymbol               , 0x4350143D267B0LL             )
X(ExportSymbol                 , 0x380047517B0LL               )
X(ReserveMemorySpace           , 0x4353443737041LL             )
X(ReservedMemory               , 0x4353443D737041LL            )
X(CommitMemory                 , 0xC0881783801LL               )
X(Execute                      , 0x3C3C073                     )

// svclinker <-> svcsymres
X(GetSymbolLibrary             , 0x63751711B                   )
X(ResolvedSymbolLibrary        , 0x435013D417                  )
