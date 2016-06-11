function GetChapterList (ComicIndex) {
  var ComicName  = "";
  var BigCodes   = "";
  
  for (var i = 0; i < gComicList.length; i++) {
    if (gComicList[i][0] == ComicIndex) {
      ComicIndex = gComicList[i][0];
      ComicName  = gComicList[i][1];
      BigCodes   = gComicList[i][2];
      break;
    }
  }
  CodesList = BigCodes.split("|");
  
  var ChapterList = new Array();
  
  for (var i = 0; i < CodesList.length; i++) {
    ChapterList[i] = new Array(
                           ComicIndex,                   // 0 ComicIndex
                           ComicName,                    // 1 ComicName
                           CodesList[i].split(' ')[0],   // 2 ChapterIndex
                           CodesList[i].split(' ')[1],   // 3 Sid
                           CodesList[i].split(' ')[2],   // 4 Did
                           CodesList[i].split(' ')[3],   // 5 ChapterRange
                           CodesList[i].split(' ')[4]    // 6 Code
                           );
  }
  return ChapterList;
}

function SetMenuList (KeyWord,ComicIndex) {

  if (KeyWord == "Chapter" && ComicIndex != undefined) {
    $("#MenuListDiv").text("");
    
    var ChapterList = GetChapterList (ComicIndex);
    
    for (var i = (ChapterList.length - 1); i >= 0; i--) {
      if (ChapterList[i][2] < 8000)
        $("#MenuListDiv").append("<div class='Item' onclick='ItemOnClick($(this),"+ ChapterList[i][0] +","+ ChapterList[i][2] +");'>第 "+ ChapterList[i][2] +" 章節</div>");
    }
    
  }
  else if (gComicList != undefined){
  
    if (gComicList.length > 0) {
      $("#MenuListDiv").text("");
      
      for (var i = 0; i < gComicList.length; i++) {
        $("#MenuListDiv").append("<div class='Item' onclick='ItemOnClick($(this));' ComicIndex='"+ gComicList[i][0] +"'>"+ gComicList[i][1] +"</div>");
      }
      
      $("#MenuTitleDiv").text ("");
      $("#MenuTitleDiv").append ("<div id='MenuTitle'>[ 漫畫 選單 ]</div>");
      $("#MenuTitleDiv").attr("value","IsComic");
      
      SetMiddleDiv("Clear");
    }
    else {
      $("#MiddleDiv").text("");
      $("#MiddleDiv").append("<img src='./Icon/Warning.png' /><hr width='40%'/>");
      $("#MiddleDiv").append("<label>※ 請先使用 OA_ComicBook.exe '選擇漫畫'並且更新漫畫完整的資訊。</label>");
    }
  }
}
function paddingLeft(str,lenght){
	if(str.length >= lenght)
	  return str;
	else
	  return paddingLeft("0" +str,lenght);
}
function GetPicName (PicIndex) {
  var Code = $("#Code").val();
  
  if ((Code != undefined) && (Code != "") && (Code.length != 0)) {
    var Key = (parseInt((PicIndex-1)/10)%10)+(((PicIndex-1)%10)*3);
    Key = Code.substring(Key,Key+3);
    PicIndex = paddingLeft(PicIndex,3);
    
    return PicIndex + "_" + Key + ".jpg";//?t="+new Date().getTime();
  }
  else {
    return undefined;
  }
}
function GetPicUrl (PicIndex) {
  var PicRange = $("#PicRange").val();
  
  if ((PicIndex <= PicRange) && (PicIndex > 0)) {
    var PicName = GetPicName (PicIndex);
    
    if (PicName != undefined) {
      return $("#BaseUrl").val() + PicName;
    }
  }
  return undefined;
}

function ChangePic (KeyWord) {
  $('body,html').animate({scrollTop:0},400);
  
  var PicIndex = $("#PicIndex").val();
  
  if (KeyWord != undefined) {
    PicIndex = (parseInt(PicIndex, 10) + (KeyWord == "Next" ?1:-1));
    var Url = GetPicUrl (PicIndex);
    if (Url != undefined) {
      
      
      $("#TopNowPic").text("目前第 "+PicIndex+" 頁");
      $("#MiddleDiv").text("");//Showpic
      
      $("#MiddleDiv").append("<img id='Image' src='./Icon/ajax-loader.gif' />");//Showpic
      $("#MiddleDiv").append("<img id='TempImage' src='"+Url+"'/>");//Showpic
      
      $('#TempImage').load(function(){
        $("#Image").attr("src",$(this).attr("src"));
      });
      
      $("#PicIndex").val(PicIndex);
      
      for (var i=0;i<5;i++) {
        PicIndex = (parseInt(PicIndex, 10) + 1);
        Url = GetPicUrl (PicIndex);
        if (Url != undefined) {
          $("#MiddleDiv").append("<img class='NextLoadImage' src='"+Url+"'/>");
        }else {
          break;
        }
      }
      
    }
  }
}
function CheckButton (){
  var PicIndex = $("#PicIndex").val();
  var PicRange = $("#PicRange").val();
  
  if ((parseInt(PicIndex, 10) + 1) <= parseInt(PicRange) && (parseInt(PicIndex, 10) + 1) > 0) {
    $(".NextButton").show();
		$(".NextButton").removeAttr("disabled");
  }
  else {
    $(".NextButton").hide();
		$(".NextButton").attr("disabled","disabled");
  }  
  if ((parseInt(PicIndex, 10) - 1) <= parseInt(PicRange) && (parseInt(PicIndex, 10) - 1) > 0) {
    $(".PrevButton").show();
		$(".PrevButton").removeAttr("disabled");
  }
  else {
      $(".PrevButton").hide();
			$(".PrevButton").attr("disabled","disabled");
  }
}

function SetMiddleDiv (KeyWord, ChapterInfomation) {
  if (KeyWord == "Browse") {
    if ((ChapterInfomation != undefined) && (ChapterInfomation.length > 0)) {
      
      var ComicIndex   = ChapterInfomation[0];
      var ComicName    = ChapterInfomation[1];
      var ChapterIndex = ChapterInfomation[2];
      var Sid          = ChapterInfomation[3];
      var Did          = ChapterInfomation[4];
      var PicRange     = ChapterInfomation[5];
      var Code         = ChapterInfomation[6];
      
      var BaseUrl = "http://img"+Sid+".8comic.com/"+Did+"/"+ComicIndex+"/"+ChapterIndex+"/";
      
      $("#BaseUrl").val(BaseUrl);
      $("#PicRange").val(PicRange);
      $("#PicIndex").val("0");
      $("#Code").val(Code);
      
      $("#TopDiv").text("");
      $("#MiddleDiv").text("");
      $("#BottomDiv").text("");
        
      $("#TopDiv").append("<div id='TopInfomation'>" + ComicName + " >> 第 " +ChapterIndex+ " 集，共 "+ PicRange +" 頁</div>");
      $("#TopDiv").append("<div id='TopNowPic'>目前第 1 頁</div>");

      $("#TopDiv").append("<div class='TopButton'><input type='button' value='上一頁' class='PrevButton' class='button' onclick='ChangePic(\"Prev\"); CheckButton();' /></div>");
      $("#TopDiv").append("<div class='TopButton'><input type='button' value='下一頁' class='NextButton' class='button' onclick='ChangePic(\"Next\"); CheckButton();' /></div>");
      $("#TopDiv").append("<hr/>");
      
      $("#BottomDiv").append("<div id='BootomInfo'>Copyright© 2012 8comic. All rights reserved. Powered by OA.</div>");
      $("#BottomDiv").append("| <div class='BootomButton'><input type='button' value='上一頁' class='PrevButton' class='button' onclick='ChangePic(\"Prev\"); CheckButton();' /></div>");
      $("#BottomDiv").append("<div class='BootomButton'><input type='button' value='下一頁' class='NextButton' class='button' onclick='ChangePic(\"Next\"); CheckButton();' /></div>");
                 
      ChangePic("Next");
      CheckButton();
    }
  }
  else if (KeyWord == "Clear") {
    $("#TopDiv").text("");
    $("#MiddleDiv").text("");
    $("#BottomDiv").text("");
    
    $("#TopDiv").append("[ 請選擇漫畫 ]");
    $("#MiddleDiv").append("<hr/>");
    $("#BottomDiv").append("<div id='BootomInfo'>Copyright© 2012 <a href='http://www.8comic.com/'>8comic</a>. All rights reserved. Powered by OA.</div>");

    $("#BaseUrl").val("");
    $("#PicRange").val("");
    $("#PicIndex").val("");
    $("#Code").val("");
  }
}
function ItemOnClick ($this, ComicIndex, ChapterIndex) {
    $this.parent().children().removeClass("ItemSelect");
    $this.addClass("ItemSelect");
   
    if ($("#MenuTitleDiv").attr("value") == "IsComic") {
      
      SetMenuList ("Chapter",$this.attr("ComicIndex"));
      
      $("#MenuTitleDiv").text ("");
      $("#MenuTitleDiv").append ("<input type='button' class='button' value='< 回 漫畫選單' onclick=\"SetMenuList('Comic');\"/>");
      
      $("#MenuTitleDiv").attr("value","IsChapter");
    }
    else {
      if ((ComicIndex != undefined) && (ChapterIndex != undefined)) {

        var ChapterInfomation = new Array();
        var ChapterList = GetChapterList (ComicIndex);
        
        for (var i = 0; i < ChapterList.length; i++) {
          if (ChapterList[i][2] == ChapterIndex) {
            ChapterInfomation = ChapterList[i];
            break;
          }
        }
        SetMiddleDiv ("Browse",ChapterInfomation);
      }
    }
}

$(function() {

	$(this).keyup( function( event ) {
		if (event.which == 39) {
			$(".NextButton").filter(":first").click();
		}
		if (event.which == 37) {
			$(".PrevButton").filter(":first").click();
		}
	});

  $("#HideShowButton").click (function (){
    var $this = $(this);
    if ($this.val() == "<< 隱藏") {
      $("#MenuListDiv").hide ("blind");
      $this.val("選單 >>");
      
      $("#MenuRootDiv").animate({
        left: "-190px"
      }, 300);
      
    }
    else {
      $("#MenuListDiv").show ("blind");
      $this.val("<< 隱藏");
      
      $("#MenuRootDiv").animate({
        left: "0px"
      }, 300);
    }
  }).click();

  $("#MenuRootDiv").show("drop");
  
 var timer;
 $(this).scroll(function(){
    var Scroll = $(this);
    clearTimeout(timer);
    timer = setTimeout (function () {
      $("#MenuRootDiv").animate({
        top: Scroll.scrollTop() + 50
      }, 300);
    },150);
  }).scroll();
  
  try {
    SetMenuList("Comic");
  }
  catch(err) {
    $("#MiddleDiv").text("");
    $("#MiddleDiv").append("<img src='./Icon/Warning.png' /><hr width='50%'/>");
    $("#MiddleDiv").append("<label>※ 請先使用 OA_ComicBook.exe 選擇漫畫並完整的更新資訊。</label>");
    $("#MiddleDiv").append("<br/><br/>");
    $("#MiddleDiv").append("<label>※ 注意! 更新漫畫過程中請勿任意中斷更新以免造成讀取錯誤。</label>");
  }
});

