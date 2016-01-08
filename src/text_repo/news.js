var Krawler = require('krawler') ;
var request = require('request');
var $$ = require("cheerio") ;
var fs = require('fs') ;


var max = 2  ;
if (!String.prototype.trim) {
    (function(){
        // Make sure we trim BOM and NBSP
        var rtrim = /^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g;
        String.prototype.trim = function () {
            return this.replace(rtrim, "");
        }
    })();
}

String.prototype.replaceAll = function(
    strTarget, // The substring you want to replace
    strSubString // The string you want to replace in.
){
    var strText = this;
    var intIndexOfMatch = strText.indexOf( strTarget );

    // Keep looping while an instance of the target string
    // still exists in the string.
    while (intIndexOfMatch != -1){
        // Relace out the current instance.HVite
        strText = strText.replace( strTarget, strSubString )

        // Get the index of any next matching substring.
        intIndexOfMatch = strText.indexOf( strTarget );
    }

    // Return the updated string with ALL the target strings
    // replaced out with the new substring.
    return( strText );
} ;

var cleanChar = function( str ) {
    var _str = "" ;
    var alph = ["ا","ب","پ","ت","ث","ج","چ","ح","خ","د", "ظ","ط","ض","ص","ش","س","ژ","ز","ر","ذ","و","ن","م","ل","گ","ک","ق","ف","غ","ع","ی" , "ي" , "ئ" , "آ" , "ؤ", "ة", "أ", "إ", "ك", "ء", "ه", " " ]
    for ( var i =0 ; i < str.length ; i ++ ) {
        if  (alph.indexOf(str[i] ) >  -1 ) {
            _str += str[i];
        }
        else {
            if ( i < str.length-1 && alph.indexOf(str[i+1]) > -1 ) {
                _str += " "
            }
        }
    }
    _str = _str.trim() ;
    return _str
        //.split(" ")
        //.slice(0 , ( str.slice(" ").length > 6 ? 6 : str.slice(" ").length ))
        //.join(" ")
} ;
var testChar = function (_str) {
    var _str = str ;
    _str = _str.replace(/‌»/g , '' ) ;
    _str = _str.replace(/«/g , '' ) ;
    _str = _str.replaceAll("'" , '' ) ;
    _str = _str.replaceAll('"' , '' ) ;
    _str = _str.replaceAll('?' , '' ) ;
    _str = _str.replaceAll('؟' , '' ) ;
    _str = _str.replaceAll('!' , '' ) ;
    _str = _str.replaceAll(')' , '' ) ;
    _str = _str.replaceAll('(' , '' ) ;
    _str = _str.replaceAll('}' , '' ) ;
    _str = _str.replaceAll('{' , '' ) ;
    _str = _str.replaceAll('{' , '' ) ;
    _str = _str.replaceAll('!' , '' ) ;
    _str = _str.replaceAll(':' , '' ) ;
    _str = _str.replaceAll(';' , '' ) ;
    _str = _str.replaceAll(',' , '' ) ;
    _str = _str.replaceAll('،' , '' ) ;
    _str = _str.replaceAll('‌+' , '' ) ;
    _str = _str.replaceAll('-' , '' ) ;
    _str = _str.replaceAll('_' , '' ) ;
    _str = _str.replaceAll('-' , '' ) ;
    _str = _str.replaceAll('/' , '' ) ;
    _str = _str.replaceAll('؛' , '' )
        .trim() ;
    return _str
        //.split(" ")
        //.slice(0 , ( str.slice(" ").length > 5 ? 5 : str.slice(" ").length ))
        //.join(" ")
} ;



var recReq = function (page) {
    console.log("start : " + page) ;
    var _page = page ;
    if ( page == max +1 ) {
        return
    }
    request.get("https://www.tabnak.ir/fa/archive?service_id=-1&sec_id=-1&cat_id=-1&rpp=100&from_date=1384/01/01&to_date=1394/09/20&p=" + _page
        ,
        function (error, response, body) {
            if (!error && response.statusCode == 200) {
                var $ = $$.load(body) ;
                $("div.linear_news a").each(function (i, item) {
                    var text  = cleanChar($(this).text()) ;
                    //data.push(text)
                    stream.write(text + "\n")
                }) ;
                console.log("page " + page + " Done " , _page ) ;
                recReq(++_page) ;
            }
        }) ;
} ;
var stream = fs.createWriteStream("news_data_" + max + ".txt");
stream.once('open', function(fd) {
    fs.writeFile("news_data_" + max + ".txt", '', function(){
	stream.write(["ا","ب","پ","ت","ث","ج","چ","ح","خ","د", "ظ","ط","ض","ص","ش","س","ژ","ز","ر","ذ","و","ن","م","ل","گ","ک","ق","ف","غ","ع","ی" , "ي" , "ئ" , "آ" , "ؤ", "ة", "أ", "إ", "ك", "ء", "ه", " " ].join(''))
        console.log('file cleared') ;
        recReq(1) ;
    }) ;

});
