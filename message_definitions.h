X(OK                           , 0x0CULL << 56                 , Empty                         )

X(Unknown                      , 0x0ULL                        , Empty                         )

// client <-> svcreqhandler
X(Version                      , 0x01ULL                       , Value<uint64_t>               )
X(Authorization                , 0x0ALL                        , Token                         )
X(LinkLibrary                  , 0x111BLL                      , LinkLibrary                   )

// client <-> svclinker
X(ResolveExternalSymbol        , 0x435014332417BLL             , ResolveExternalSymbol         )
X(ResolvedSymbol               , 0x4350143D267B0LL             , ResolvedSymbol                )
X(ReserveMemorySpace           , 0x4353443737041LL             , ReserveMemorySpace            )
X(ReservedMemory               , 0x4353443D737041LL            , ReservedMemory                )
X(CommitMemory                 , 0xC0881783801LL               , CommitMemory                  )
X(Execute                      , 0x3C3C073                     , Value<uint64_t>               )

// svclinker <-> svcsymres
X(GetSymbolLibrary             , 0x63751711B                   , GetSymbolLibrary              )
X(ResolvedSymbolLibrary        , 0x435013D417                  , ResolvedSymbolLibrary         )

