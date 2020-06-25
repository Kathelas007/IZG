#include <assert.h>

#include<student/window.hpp>
#include<student/application.hpp>
#include<student/emptyMethod.hpp>
#include<student/triangleMethod.hpp>
#include<student/triangleClip1Method.hpp>
#include<student/triangleClip2Method.hpp>
#include<student/triangle3DMethod.hpp>
#include<student/triangleBufferMethod.hpp>
#include<student/czFlagMethod.hpp>
#include<student/phongMethod.hpp>
#include<tests/conformanceTests.hpp>
#include<tests/performanceTest.hpp>
#include<tests/takeScreenShot.hpp>

#include<student/arguments.hpp>

int main(int argc,char*argv[]){
  try{
    auto args = Arguments(argc,argv);
    if(args.stop)
      return 0;

    if(args.runConformanceTests){
      runConformanceTests(args.groundTruthFile);
      return 0;
    }

    if(args.runPerformanceTests){
      runPerformanceTest(args.perfTests);
      return 0;
    }

    if(args.takeScreenShot){
      takeScreenShot(args.groundTruthFile);
      return 0;
    }

    auto app = Application(args.windowSize[0],args.windowSize[1]);
    app.registerMethod<EmptyMethod         >("empty window"                                     );
    app.registerMethod<TriangleMethod      >("triangle 2D"                                      );
    app.registerMethod<TriangleClip1Method >("triangle clipping (one point behind near plane)"  );
    app.registerMethod<TriangleClip2Method >("triangle clipping (two points behind near plane)" );
    app.registerMethod<Triangle3DMethod    >("triangle 3D"                                      );
    app.registerMethod<TriangleBufferMethod>("triangle stored in buffer"                        );
    app.registerMethod<CZFlagMethod>        ("czech flag"                                       );
    app.registerMethod<PhongMethod         >("phong bunny"                                      );
    app.setMethod(args.method);
    app.start();

  }catch(std::exception&e){
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}

/**
 * \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 *
 * Buffer je lineárlní paměť na grafické kartě.
 * Uživatel si může na grafické kartě vytvořit libovolné množství bufferů o libovolné velikosti.
 * Buffer slouží pro uložení dat vrcholů a indexů.
 * Jednotlivé buffery jsou identifikovány pomocí celočíselného identifikátoru: \link BufferID \endlink.
 * Buffer je možné vytvořit, smazat, nahrát do něj data, vyčíst z něj data a zjistit, zda daný identifiktár odkazuje na existující buffer.
 * Identifikátor bufferu musí být unikátní.
 * \link emptyID \endlink je prázdný identifikátor, který nesmí být použit pro existující buffer.
 *
 */





/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * <b>Vertex Puller</b> je zařízení na grafické kartě, které se stará o sestavení vrcholů.
 * Vrchol (\link InVertex \endlink) je složen z \link maxAttributes \endlink vertex attributů, každý může být různého typu (\link AttributeType \endlink (float, vec2, vec3, vec4), a čísla vrcholu.
 * Vertex puller je složen z \link maxAttributes \endlink čtecích hlav, které sestavují jednotlivé vertex attributy.
 * Dalším úkolem vertex pulleru je spočítat číslo vrcholů \ref InVertex::gl_VertexID.
 * <br>
 * Číslo vrcholu je dáno pořadovým číslem vyvolání (invokací) vertex shaderu - to v případě že není použito indexování.
 * Jinak je číslo dáno indexem v indexačnímu bufferu.
 * Indexační buffer může mít růzou velikost indexu - 8bit, 16bit a 32bit (\link IndexType \endlink).
 * Pokud je zapnuto indexování, pak je číslo vrcholu dáno položkou v indexačním bufferu, kde 
 * položka (index) v bufferu je vybráno na základě čísla invokace vertex shaderu.
 * Dalším úkolem vertex pulleru je připravit aributy vrcholu, který vstupuje do vertex shaderu \link InVertex \endlink.
 * \link InVertex \endlink je složen z \link maxAttributes \endlink atributů, každý odpovídá jedné čtecí hlavě z vertex pulleru.
 * Čtecí hlava obsahuje nastavení - offset, stride, size a buffer id.
 * Pokud je čtecí hlava povolena, měla by zkopírovat data (o velikosti vertex atributu) z bufferu od daného offsetu, s krokem stride.
 * Všechny velikosti jsou v bajtech.
 * Krok se použije při čtení různých vrcholů: atributy by měly být čteny z adresy: buf_ptr + offset + stride*gl_VertexID
 * <br>
 * \image html images/pullerSettings.svg "Data uložená v tabulce nastavení vertex pulleru."
 *
 * Na dalších dvou obrázcích je příklad stavu vertex pulleru ve dvou (0. a 1.) invokaci vertex shaderu.
 * \image html images/puller0.svg "příklad vertex pulleru při 0. invokací vertex shaderu. Vertex je složen z 3 vertex atirbutů (coord, normal, position). Čtecí hlavy začínají na daných offsetech a v daných bufferech"
 * \image html images/puller1.svg "příklad vertex pulleru při 1. invokací vertex shaderu. Čtecí hlavy se posunuly o krok (stride)."
 *
 * <b>Indexované kreslení</b> je způsob snížení redundance dat s využitím indexů
 * na vrcholy.
 * U neindexovaného kreslení je číslo vrcholu \ref InVertex::gl_VertexID rovno číslu invokace vertex shaderu.
 * U indexovaného kreslení je číslo vrcholu \ref InVertex::gl_VertexID rovno hodnodě z indexačního bufferu.
 * Hodnota z indexačního bufferu je vybrána číslem invokace vertex shaderu.
 *
 * \image html images/drawElements.svg "Neindexované a indexované kreslení."
 */




/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * <b>Shader Program</b> je kolekce shaderů, které běží na programovatelných
 * částech zobrazovacího řetězce.
 * Obsahuje vždy maximálně jeden shader daného typu.
 * V tompto projektu je program reprezentován pomocí dvou ukazatelů na funkce \link VertexShader \endlink a \link FragmentShader \endlink.
 * K danému programuje je ještě obvykle připojena tabulka uniformních proměnných \link Uniforms \endlink. To jsou konstanty (obsahují obvykle matice, pozici světla apod.)
 * Jelikož v tomto projektu jsou shadery reprezentovány pomocí funkcí (a ne pomocí GLSL kódu) je nutné ještě sdělit, jaké vertex atributy by se měli interpolovat do fragment atributů.
 * \image html images/shader_program.svg "Shader program složený z vertex shaderu a fragment shaderu"
 */




/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * <b> Framebuffer </b> je kus paměti na grafické kartě do kterého se ukládá barva a hloubka fragmentu.
 * Je složen z dvou částí - color buffer, depth buffer.
 * Barevný buffer v tomto projektu je složen z pixelů, které mají 4 kanály reprezentované pomocí uint8_t.
 * 0 znamená nejmenší intenzita, 255 znamená maximální intenzita.
 * Hloubkový buffer v tomto projektu je složen z pixelů, které mají 1 kanál reprezentovaný pomocí float.
 * Pořadí a formát pixelů je naznačen na následujícím obrázku:
 *
 * \image html images/framebuffer.svg "Framebuffer je složený z color a depth bufferu. Oba mají stejné rozlišení. Barva je uložena v 4x uint8_t, kde jednotlivé bajty odpovídají červenému, zelenému, modrému a průhlednému kanálu. Hloubka je uložena v 1x float."
 */



/**
 * \mainpage Izg project.
 *
 * \section zadani Zadání projektu do předmětu IZG.
 *
 * \tableofcontents
 *
 * \image html images/output.png "Výstup projektu"
 *
 * Vašim úkolem je naimplementovat jednoduchou grafickou kartu (gpu).
 * Pomocí gpu vizualizovat model králička s phongovým
 * osvělovacím modelem a phongovým stínováním s procedurální texturou.
 * Cílem projektu je naučit vás jak fungují dnešní grafické karty a jak tyto karty ovládat pomocí API (které je blízké k OpenGL).
 * Každý úkol má přiřazen akceptační test, takže si můžete snadno ověřit funkčnosti vaší implementace.
 * Dále doporučuji prozkoumání, jak je tento projekt udělán.
 * Můžete se naučit jak udělat CMake, jak udělat unit testy, jak psát doxygen. Klidně se mě můžete na tyto věci zeptat.
 * <br>
 * Úkol je složen ze dvou částí:
 * <ol>
 * <li> implementace GPU
 * <li> využití GPU pro vykreslení otexturovaného králíčka vrámci PhongMethod vykreslovací metody.
 * </ol>
 * \image html images/tasks.svg "Hlavní rozdělení úkolů" width=800
 * <br>
 * Všechny úkoly naleznete zde: <a href="modules.html">kategorie úkolů</a> nebo v kompletním výpisu \link todo.html \endlink.
 *
 * \subsection Overview Vytváření objektů na grafické kartě (1a. úkol)
 * V tomto úkolu je potřeba implementovat obslužné funkcé pro objekty na grafické kartě.
 * Stači editovat soubory \ref gpu.hpp a \ref gpu.cpp.
 * <br>
 * <br>
 * Grafická karta je složena z paměti a výpočetního / vykreslovacího řetězce (pipeline).
 * \image html images/gpu_memory_pipeline.svg "základní schéma grafické karty." width=600
 * V paměti je uloženo několik informací. Jsou to:
 * <ul>
 * <li> Buffery
 * <li> Tabulky s nastavením vertex pullerů
 * <li> Shader programy a jejich nastavení včetně uniformích proměnných
 * <li> Framebuffer
 * </ul>
 * \image html images/gpu_memory.svg "Objekty uložené v paměti" width=400
 * Ke každému typu objektu se vážou obslužné funkce.
 * Tyto funkce můžou vytvořit nové objekty.
 * Náhrát do nich data.
 * Vyčíst z nich data nebo je smazat.
 * Každý objekt je identifikován pomocí celočíselného identifikátoru \link BufferID \endlink, \link ProgramID \endlink, \link VertexPullerID \endlink.
 * Objekt který neexistuje má identifikátor \link emptyID \endlink.
 * Situace je znázorněna na následujícím obrázku:
 *
 * \image html images/gpu_id.svg "Objekty na GPU jsou identifikovány pomocí celočíselných identifikátorů. Při vytvoření objektu na grafické kartě je na cpu stranu vrácen identifikátor, pomocí kterého je možné s objektem operovat." width=800
 *
 * K těmto 4 objektům se vážou 4 podčásti úkolu:
 * <ul>
 * <li> \ref buffer_tasks "Implementace obslužných funkcí pro buffery"
 * <li> \ref vertexpuller_tasks "Implementace obslužných funkcí pro vertex pullery"
 * <li> \ref program_tasks "Implementace obslužných funkcí pro shader programy"
 * <li> \ref framebuffer_tasks "Implementace obslužných funkcí pro framebuffer"
 * </ul>
 *
 * Po implementaci těchto 4 úkolu by mělo být možné na grafické kartě vytvářet objekty a upravovat je.
 * Pro implementaci těchto úkolů můžete vytvářet proměnné uvnitř třídy GPU a inicializovat je v konstruktoru a deinicializovat je v destruktoru této třídy:
 * <ul>
 * <li> \ref gpu_init "proměnné, inicializace, deinicializace".
 * </ul>
 *
 * Po implementaci obslužných funkcí pro objekty můžete implementovat vykreslovací funkce - funkcionalitu vykreslovacího řetězce.
 * \subsection Draw Kreslení (1b. úkol)
 * Další úkoly se týkají kreslení.
 * Stejně jako předchozí úkol, je potřeba editovat akorát soubory \ref gpu.cpp a \ref gpu.hpp.
 * Je nutné naprogramovat 2 kreslící funkce.
 * První pouze vymaže framebuffer \ref GPU::clear, druhá vyrasterizuje seznam trojúhelníků \ref GPU::drawTriangles.
 * V této části je popsána teorie kreslení na grafických kartách, kterou budete muset zreplikovat v kódě.
 * \subsubsection Pipeline
 * Vykreslovací řetězec je zobrazen na následujícím obrázku:
 * \image html images/rendering_pipeline.svg "Simplified rendering pipeline"
 * Všechny části tohoto obrázku je potřeba implementovat vrámci kreslící funkce pro kreslení trojúhelníků, jmenovitě:
 * <ol>
 * <li> Vertex Puller
 * <li> Vertex Processor
 * <li> Primitive Assembly
 * <li> Clipping
 * <li> Perspective Division
 * <li> Viewport Transformation
 * <li> Rasterization
 * <li> Fragment Processor
 * <li> Per-Fragment Operation (Depth test)
 * </ol>
 * \subsubsection VertexPuller Vertex Puller
 * Detailnější popis, co dělá vertex puller naleznete v úkolu \ref vertexpuller_tasks "Implementace obslužných funkcí vertex pulleru".
 * Hlavní účel vertex pulleru je sestavení vrcholů pro vertex procesor.
 * Vertex puller je zařízení, které vyčte data z několik bufferů a staví vrchol (struktura několika vertex atributů).
 * Úživatel grafické karty má možnost měnit nastavení vertex pulleru.
 * Může měnit počet vertex atributů v sestavovaném vrcholu.
 * Může měnit buffery, ze kterých jsou jednotlivé vertex atributy čteny, může měnit způsob čtení (offset a stride (krok)).
 * Toto nastavení je uloženo v tabulce (často se ji říká VAO (vertex array object)).
 * Úživatel může na grafické kartě vytvořit několik takovýchto tabulek s nastavením a libovolně mezi nimi přepínat.
 * \image html images/VertexPuller.svg "Vizualizace funkcionality vertex pulleru. Nastavení vertex pulleru je uloženo na grafické kartě. Uživatel si může vybrat jedno nastavení. Vertex Puller podle daného nastavení sestaví vrchol,. Vertex Puller čte data z bufferů, kterým se říká Vertex Buffer. Případně je čtení řízeno ještě indexy z Index Bufferu." width=800
 *
 * \subsubsection VertexProcessor Vertex Processor
 * Úkolem vertex processoru je transformace vrcholů pomocí transformačních matic.
 * Vertex processor vykonává shader (kus programu), kterému se říká vertex shader.
 * Vstupem vertex shaderu je vrchol \ref InVertex, výstupem je vrchol \ref OutVertex.
 * Dalším (konstatním) vstupem vertex shaderu jsou uniformní proměnné \ref Uniforms, které jsou uložené v rámci shader programu.
 * Pokud se uživatel rozhodne vykreslit 5 trojúhelníků je vertex shader spuštěn \f$ 5 \cdot 3 = 15 \f$.
 * Jednotlivé spuštění (invokace) vertex shaderu vyžadují nové vstupní vrcholy a produkují nové výstupní vrcholy.
 * To ve výsledku znamená, že se pro každou invokaci vertex shaderu spustí vertex puller, který sestaví vstupní vrchol.
 * \image html images/vertexShader.svg "Vizualizace vstupů a výstupů vertex procesoru. Ve vertex procesoru běží vertex shader, který obdrží vstupní vrchol, vyprodukuje výstupní vrchol a obdrží vstupní konstanty (uniformní proměnné)." width=800
 *
 * Vertex Shader je v tomto projektu reprezentován jako C++ funkce.
 * Tato funkce má pevný typ \ref VertexShader.
 * Příklad jak může vypadat takový vertex shader můžete najít třeba tady \ref czFlag_VS.
 *
 * \subsubsection PrimitiveAssembly Primitive Assembly
 * Primitive Assembly je jednotka která sestavuje trojúhelníky (mimo jiné).
 * Trojúhelníku, úsečce, bodu se hromadně říká primitivum.
 * V tomto projektu se používají pouze trojúhelníky.
 * Primitive Assembly jednotka si počká na 3 po sobě jdoucí \ref OutVertex "výstupní vrcholy" z vertex shaderu a sestaví trojúhelník (struktura která by měla obsahovat 3 výstupní vrcholy).
 * Lze na to také nahlížet tak, že primitiv assembly jednotka dostane příkaz vykreslit třeba 4 trojúhelníky. Jednotka tak spustí vertex shader 12x, který takto spustí 12x vertex puller.
 *
 * \image html images/PrimitiveAssembly.svg "Vizualizace funkce primitive assembly jednotky. Primitive assembly jednotka sestaví trojúhelník ze 3 po sobě jdoucích výstupních vrcholů z vertex shaderu." width=800
 *
 * \subsubsection Clipping
 * Ořez (clipping) slouží pro odstranění částí trojúhelníků, které leží mimo pohledový jehlan.
 * Nejdůležitější je však ořet near ořezovou rovinou pohledoveho jehlanu.
 * Pokud by se neprovedl ořez pomocí near roviny, pak by se vrcholy nebo i celé trojúhělníky, které leží za středem projekce promítly při perspektivním dělení na průmětnu.
 * Ořez se provádí v clip-space - po pronásobení vrcholů projekční maticí.
 * Pro body, které leží uvnitř pohledového tělesa platí, že jejich souřadnice splňují následující nerovnice: \f$ -A_w \leq A_i \leq +A_w \f$, \f$i \in \left\{ x,y,z \right\}\f$.
 * Těchto 6 nerovnic reprezentuje jednotlivé svěny pohledového jehlanu.
 * Nerovnice \f$ -A_w \leq A_z \f$ reprezentuje podmínku pro near ořezovou rovinu.
 * <br>
 * Při ořezu trojúhelníku můžou nastat 4 případy, jsou znázorněny na následujícím obrázku:
 *
 * \image html images/clip_variants.svg "4 varianty ořezu trojúhelníku pomocí near roviny. Počet vrcholů, které leží před ořezovou rovinou určuje typ ořezu. Při ořezu může vzniknout 0 až 2 nové trojúhelníky."
 *
 * Ořez trojúhelníku pomocí near roviny lze zjednodušit na ořez hran trojúhelníku.
 * Bod na hraně (úsečce) trojúhelníku lze vyjádřit jako: \f$ \overrightarrow{X(t)} = \overrightarrow{A} + t \cdot (\overrightarrow{B}-\overrightarrow{A}) \f$, \f$t \in [0,1] \f$.
 * \f$ \overrightarrow{A}, \overrightarrow{B} \f$ jsou vrcholy trojúhelníka, \f$ \overrightarrow{X(t)} \f$ je bod na hraně a parametr \f$ t \f$ udává posun na úsečce.
 *
 * \image html images/clip.svg "Ořez trojúhelníku pomocí ořezu hran. Při ořezu hran vzniknou nové body, ze kterých jsou následně sestaveny nové trojúhelníky."
 *
 * Souřadnice bodu \f$ \overrightarrow{X(t)} \f$ lze určit při vypočtení parametru \f$ t \f$, při kterém přestane platit nerovnice pro near rovinu \f$ -X(t)_w \leq X(t)_z \f$.
 * Takové místo nastává v situaci \f$ -X(t)_w = X(t)_z \f$.
 * Po dosazení z rovnice úsečky lze vztah přepsat na: 
 * \f{eqnarray*}{
 * -X(t)_w &=& X(t)_z \\
 * 0 &=& X(t)_w + X(t)_z \\
 * 0 &=& A_w + t \cdot (B_w-A_w) + A_z + t \cdot (B_z - A_z) \\
 * 0 &=& A_w + A_z + t \cdot (B_w-A_w+B_z-A_z) \\
 * -A_w-A_z &=& t \cdot (B_w-A_w+B_z-A_z) \\
 * \frac{-A_w - A_z}{B_w-A_w+B_z-A_z} &=& t\\
 * \f}
 *
 * Pozice bodu \f$ \overrightarrow{X(t)} \f$ a hodnoty dalších vertex atributů lze vypočítat lineární kombinací hodnot z vrcholů úsečky pomocí parametru \f$ t \f$ následovně: \f$ \overrightarrow{X(t)} = \overrightarrow{A} + t \cdot (\overrightarrow{B}-\overrightarrow{A}) \f$.
 *
 * \subsubsection PerspectiveDivision Perspektivní dělení
 * Perspektivní dělení následuje za clippingem a provádí převod z homogenních souřadnic na kartézské pomocí dělení w.
 * \image html images/PerspectiveDivision.svg "Perspektivní dělení. Převod z clip-space do NDC (normalized device coordinates). Dělí se pomocí w. Při tomto dělení vzniknou normalizované souřadnice x,y a normalizovaná hloubka." width=500
 *
 * \subsubsection ViewPortTransformation Viewport transformace
 * Viewport transformace provádí převod NDC (rozsah -1, +1) na rozlišení okna, aby se mohla provést rasterizace.
 * \image html images/ViewportTransformation.svg "Vizualizace viewport transformace. Trojúhelníky jsou zoztaženy na rozlišení obrazovky [width,height]." width=500
 *
 *
 * \subsubsection Rasterization Rasterizace
 * Rasterizace rasterizuje trojúhelník ve screen-space.
 * Rasterizace produkuje fragmenty v případě, že <b>střed</b> pixelu leží uvnitř trojúhelníka.
 * Rasterizace by měla zapsat souřadnice fragmentu do proměnné \ref InFragment::gl_FragCoord ve struktuře \ref InFragment.
 * Pozice fragmentu obsahuje v x,y souřadnici na obrazovce a v z hloubku.
 * <br>
 * Další úkol rasterizace je interpolace vertex attributů do fragment attributů.
 * Atributy (a jejich typ) které jsou posílány z vertex shaderu do fragment shaderu si uživatel může zvolit funkcí \ref GPU::setVS2FSType.
 * Úkolem rasterizéru je perspektivně korektně interpolovat atributy.
 * Perspektivně korektní interpolace využívá pro interpolaci barycentrické koordináty spočítané z 3D trojúhelníku a ne z 2D trojúhelníku.
 * Interpolaci je možné provést i pomocí 2D barycentrických koordinát při použití perspektivní korekce:
 *
 * \f[\displaystyle \frac{\frac{A_0 \cdot \lambda_0}{h_0} + \frac{A_1 \cdot \lambda_1}{h_1} + \frac{A_2 \cdot \lambda_2}{h_2}}{\frac{\lambda_0}{h_0}+\frac{\lambda_1}{h_1}+\frac{\lambda_2}{h_2}}\f]
 * Kde \f$\lambda_0,\lambda_1,\lambda_2\f$ jsou barycentrické koordináty ve 2D, \f$h_0,h_1,h_2\f$ je homogenní složka vrcholů a \f$A_0,A_1,A_2\f$ je atribut vrcholu.
 *
 * \image html images/interpolation.svg "Rasterizace a interpolace vertex atributů. Vertex Atributy jsou interpolovány pomocí perspektivně korektních barycentrických souřadnic w0,w1,w2. Fragmenty vznikají pouze tehdy, pokud jejich střed leží uvnitř trojúhelníku."
 *
 * \subsubsection FragmentProcessor Fragment processor
 * Fragment processor spouští fragment shader nad každým fragmentem.
 * Data pro fragment shader jsou uložena ve struktuře \ref InFragment.
 * Výstup fragment shaderu je výstupní fragment \ref OutFragment - barva.
 * Další (konstantní) vstup fragment shaderu jsou uniformní proměnné.
 * \image html images/FragmentShader.svg "Vizualizace vstupů a výstupů fragment shaderu. Fragment Shader se pouší nad každým vyrasterizovaným fragmentem." width=800
 *
 *
 * \subsubsection PFO Per-fragment operace
 * Per-fragment operace provádí depth test.
 * Ověření zda je nový fragment blíže než hloubka poznačená ve framebufferu.
 * Pokud je hloubka nového fragment menší, barva a hloubka fragmentu je zapsána do framebufferu.
 * Dejte pozor na přetečení rozsahu gl_FragColor. Před zápisem je nutné ořezat barvu do rozsahu <0,1>.
 *
 * \image html images/PFO.svg "Vizualizace depth testu. Starý fragment (jeho barva a hloubka) je nahrazen pokud nový fragment má menší hloubku." width=800
 *
 *
 * \subsection PhongMethod Implementace vykreslovací metody PhongMethod (úkol 2.)
 * Druhý úkol je implementace vykreslování králička s phongovým osvětlovacím modelem a phognovým stínováním s procedurální texturou.
 * Úkol je složen ze dvou částí:
 * <ul>
 * <li> \ref shader_side "Implementace vertex a fragment shaderu"
 * <li> \ref cpu_side "Implementace inicializace vykreslování a samotného kreslení"
 * </ul>
 * Pro implemenatci si můžete inspirovat příklady: \ref triangleMethod.cpp \ref triangle3DMethod.cpp, \ref triangleBufferMethod.cpp, \ref czFlagMethod.cpp.
 *
 * \image html images/examples.svg "příklady" width=1000
 *
 *
 * V projektu jsou přítomny i nějaké další příklady.
 * Tyto příklady můžete využít pro inspiraci a návod jak napsat vykreslování a
 * shadery.
 *
 * \subsection terminologie Terminologie
 *
 * \b Vertex je kolekce několika vertex atributů.
 * Tyto atributy mají svůj typ a počet komponent.
 * Každý vertex atribut má nějaký význam (pozice, hmotnost, texturovací
 * koordináty), které mu přiřadí programátor/modelátor.
 * Z několika vrcholů je složeno primitivum (trojúhelník, úsečka, ...)
 *
 * <b>Vertex atribut</b> je jedna vlastnost vrcholu (pozice, normála,
 * texturovací koordináty, hmotnost, ...).
 * Atribut je složen z 1,2,3 nebo 4 komponent daného typu (FLOAT, INT, ...).
 * Sémantika atributu není pevně stanovena (atributy mají pouze pořadové číslo -
 * attribIndex) a je na každém programátorovi/modelátorovi, jakou sémantiku
 * atributu přidělí.
 * \image html images/primitive.svg "Vertex attributes"
 *
 * \b Fragment je kolekce několika atributů (podobně jako Vertex).
 * Tyto atributy mají svůj typ a počet komponent.
 * Fragmenty jsou produkovány resterizací, kde jsou atributy fragmetů vypočítány
 * z vertex atributů pomocí interpolace.
 * Fragment si lze představit jako útržek původního primitiva.
 *
 * <b>Fragment atribut</b> je jedna vlastnost fragmentu (podobně jako vertex
 * atribut).
 *
 *
 * <b>Vertex Processor</b> (často označován za Vertex Shader) je funkční blok,
 * který je vykonáván nad každým vertexem.
 * Jeho vstup i výstup je Vertex. Výstupní vertex má obvykle jiné vertex
 * atributy než vstupní vertex.
 * Výstupní vertex má vždy atribut - gl_Position (pozice vertexu v clip-space).
 * Vstupní vertex má vždy atribut - gl_VertexID (číslo vrcholu, s ohledem na indexování).
 * Vertex Processor se obvykle stará o transformace vrcholů modelu (posuny,
 * rotace, projekce).
 * Jelikož Vertex Processor pracuje po vrcholech, je vhodný pro efekty jako
 * vlnění na vodní hladině, displacement mapping apod.
 * Vertex Processor má informace pouze o jednom vrcholu v daném čase (neví nic o
 * sousednostech vrcholů).
 * Vertex processor je programovatelný.
 *
 * <b>Fragment Processor</b> (často označován za Fragment Shader/Pixel Shader)
 * je funkční blok, který je vykonáván nad každým fragmentem.
 * Jeho vstup i výstup je Fragment. Výstupní fragment má obykle jiné attributy.
 * Fragment processor je programovatelný.
 *
 * <b>Shader</b> je program/funkce, který běží na některé z programovatelných
 * částí zobrazovacího řetezce.
 * Shader má vstupy a výstupy, které se mění s každou jeho invokací.
 * Shader má také vstupy, které zůstávají konstantní a nejsou závislé na číslu
 * invokace shaderu.
 * Shaderů je několik typů, v tomto projektu se používají pouze 2 - vertex
 * shader a fragment shader.
 * V tomto projektu jsou shadery reprezentovány pomocí standardních Cčkovských
 * funkcí.
 *
 * <b>Vertex Shader</b> je program, který běží na vertex processoru.
 * Jeho vstupní interface obsahuje: vertex, uniformní proměnné a další proměnné
 * (číslo vrcholu gl_VertexID, ...).
 * Jeho výstupní inteface je vertex, který vždy obsahuje proměnnou gl_Position -
 * pozici vertexu v clip-space.
 *
 * <b>Fragment Shader</b> je program, který běží na fragment processoru.
 * Jeho vstupní interface obsahuje: fragment, uniformní proměnné a proměnné
 * (souřadnici fragmentu ve screen-space gl_FragCoord, ...).
 * gl_FragCoord.xy - souřadnice ve screen space
 * gl_FragCoord.z - hloubka
 * Jeho výstupní interface je fragment.
 * V projektu obsahuje atribut gl_FragColor - pro výstupní barvu.
 *
 * <b>Buffer</b> je lineární pole dat ve video paměti na GPU.
 * Do bufferů se ukládají vertex attributy vextexů modelů nebo indexy na vrcholy
 * pro indexované vykreslování.
 *
 * <b>Uniformní proměnná</b> je proměná uložená v konstantní paměti GPU. Všechny
 * programovatelné bloky zobrazovacího řetězce z nich mohou pouze číst.
 * Jejich hodnota zůstává stejná v průběhu kreslení (nemění se v závislosti na
 * číslu vertexu nebo fragmentu). Jejich hodnodu lze změnit z CPU strany pomocí
 * funkcí jako je
 * uniform1f, uniform1i, uniform2f, uniformMatrix4fv apod. Uniformní proměnné
 * jsou vhodné například pro uložení transformačních matic nebo uložení času.
 *
 * <b>Vertex Puller</b> se stará o přípravů vrcholů.
 * K tomuto účelu má tabulku s nastavením.
 * Vertex puller si můžete představit jako sadu čtecích hlav.
 * Každá čtecí hlava se stará o přípravu jednoho vertex atributu.
 * Mezi nastavení čtecí hlavy patří: ukazatel na začátek bufferu, offset a krok.
 * Vertex puller může obsahovat indexování.
 *
 *
 * \section rozdeleni Rozdělení
 *
 * Projekt je rozdělen do několika podsložek:
 *
 * <b>student/</b> Tato složka obsahuje soubory, které využijete při
 * implementaci projektu.
 * Složka obsahuje soubory, které budete odevzávat a podpůrné knihovny.
 * Všechny soubory v této složce jsou napsány v jazyce C++ abyste se mohli podívat
 * jak jednotlivé části fungují.
 *
 * <b>tests/</b> Tato složka obsahuje akceptační a performanční testy projektu.
 * Akceptační testy jsou napsány s využitím knihovny catch.
 * Testy jsou rozděleny do testovacích případů (TEST_CASE). Daný TEST_CASE
 * testuje jednu podčást projektu.
 *
 * <b>doc/</b> Tato složka obsahuje doxygen dokumentaci projektu.
 * Můžete ji přegenerovat pomocí příkazu doxygen spuštěného v root adresáři
 * projektu.
 *
 * <b>images/</b> Tato složka obsahuje doprovodné obrázky pro dokumentaci v
 * doxygenu.
 * Z pohledu projektu je nezajímavá.
 *
 * Složka student/ obsahuje soubory, které se vás přímo týkají:
 *
 * \ref gpu.hpp a \ref gpu.cpp obsahuje deklaraci a definici funkcí grafické karty - tady odvedete nejvíce práce.
 *
 * \ref phongMethod.hpp a \ref phongMethod.cpp obsahuje vykresleni králička - toto máte taky naprogramovat.
 *
 * \ref fwd.hpp obsahuje definice typů a konstanty - projděte si.
 *
 * Projekt je postaven nad filozofií OpenGL. Spousta funkcí má podobné jméno.
 *
 *
 * \section sestaveni Sestavení
 *
 * Projekt byl testován na Ubuntu 18.04, Visual Studio 2017, 2019.
 * Projekt vyžaduje 64 bitové sestavení.
 * Projekt využívá build systém <a href="https://cmake.org/">CMAKE</a>.
 * CMake je program, který na základně konfiguračních souborů "CMakeLists.txt"
 * vytvoří "makefile" v daném vývojovém prostředí.
 * Dokáže generovat makefile pro Linux, mingw, solution file pro Microsoft
 * Visual Studio, a další.
 * Postup:
 * -# stáhnout projekt
 * -# rozbalit projekt
 * -# ve složce build spusťte "cmake-gui .." případně "ccmake .."
 * -# vyberte si překladovou platformu (64 bit).
 * -# configure
 * -# generate
 * -# make nebo otevřete vygenerovnou Microsoft Visual Studio Solution soubor.
 *
 * \section spousteni Spouštění
 *
 * Projekt je možné po úspěšném přeložení pustit přes aplikaci
 * <b>izgProject</b>.
 * Projekt akceptuje několik argumentů příkazové řádky, pro jejich výpis použijte parametr <b> -h </b>
 * - <b>-c ../tests/output.bmp</b> spustí akceptační testy, soubor odkazuje na
 * obrázek s očekávaným výstupem.
 * - <b>-p</b> spustí performanční test.
 *
 * \section ovladani Ovládání
 * Program se ovládá pomocí myši a klávesnice:
 * - stisknuté levé tlačítko myši + pohyb myší - rotace kamery
 * - stisknuté pravé tlačítko myši + pohyb myší - přiblížení kamery
 * - "n" - přepne na další scénu/metodu
 *   "p" - přepne na předcházející scénu/metodu
 *
 * \section odevzdavani Odevzdávání
 *
 * Před odevzdáváním si zkontrolujte, že váš projekt lze přeložit na merlinovi.
 * Zkopirujte projekt na merlin a spusťte skript: <b>./merlinCompilationTest.sh</b>.
 * Odevzdávejte pouze soubory gpu.hpp, gpu.cpp, phongMethod.hpp a phongMethod.cpp
 * Soubory zabalte do archivu proj.zip. Po rozbalení archivu
 * se <b>NESMÍ</b> vytvořit žádná složka. Příkazy pro ověření na Linuxu: zip
 * proj.zip gpu.hpp gpu.cpp phongMethod.cpp phongMethod.hpp, unzip proj.zip.
 * Studenti pracují na řešení projektu samostatně a každý odevzdá své vlastní
 * řešení.
 * Poraďte si, ale řešení vypracujte samostatně!
 *
 * \section hodnoceni Hodnocení
 *
 * Množství bodů, které dostanete, je odvozeno od množství splněných
 * akceptačních testů a podle toho, zda vám to kreslí správně (s jistou
 * tolerancí kvůli nepřesnosti floatové aritmetiky).
 * Automatické opravování má k dispozici větší množství akceptačních testů
 * (kdyby někoho napadlo je obejít).
 * Pokud vám aplikace spadne v rámci testů, dostanete 0 bodů.
 * Pokud aplikace nepůjde přeložit, dostanete 0 bodů.
 *
 * \section soutez Soutěž
 *
 * Pokud váš projekt obdrží plný počet bodů, bude zařazen do soutěže o
 * nejrychlejší implementaci zobrazovacího řetězce.
 * Můžete přeimplementovat cokoliv v odevzdávaných souborech pokud to projde
 * akceptačními testy a kompilací.
 *
 * \section zaver Závěrem
 * Ať se dílo daří a ať vás grafika alespoň trochu baví!
 * V případě potřeby se nebojte zeptat (na fóru nebo napište přímo vedoucímu
 * projektu imilet@fit.vutbr.cz).
 */
