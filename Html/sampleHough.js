var btnTraitement1,
    btnTraitement2,
    btnTraitement3,
	btnTraitement4,
	ctx,
	image,
	base,
	canvasTraitement1,
	ctxTraitement1,
	canvasTraitement2,
	ctxTraitement2,
	canvasTraitement3,
	ctxTraitement3,
	canvasTraitement4,
	ctxTraitement4;
var nbTeta=180;
var nbRo=200;	

var gris = new Array(320*240);
var gris2 = new Array(320*240);
var contour = new Array(320*240);
var hough = new Array(nbRo*nbTeta);

function cestparti() {
	image = document.getElementById("image");
	image.crossOrigin = 'Anonymous';
	base = document.getElementById("canvasBase");
	ctxBase = base.getContext("2d");

	canvasTraitement1 = document.getElementById("canvasTraitement1");
	ctxTraitement1 = canvasTraitement1.getContext("2d");
	canvasTraitement2 = document.getElementById("canvasTraitement2");
	ctxTraitement2 = canvasTraitement2.getContext("2d");
	canvasTraitement3 = document.getElementById("canvasTraitement3");
	ctxTraitement3 = canvasTraitement3.getContext("2d");
	canvasTraitement4 = document.getElementById("canvasTraitement4");
	ctxTraitement4 = canvasTraitement4.getContext("2d");
	
	btnTraitement1=document.getElementById("traitement1");
	btnTraitement2=document.getElementById("traitement2");
	btnTraitement3=document.getElementById("traitement3");
	btnTraitement4=document.getElementById("traitement4");
	
	btnTraitement1.onclick=traitement1;
	btnTraitement2.onclick=traitement2;
	btnTraitement3.onclick=traitement3;
	btnTraitement4.onclick=traitement4;
	
	ctxBase.drawImage(image,0,0);
	ctxBase.crossOrigin = 'anonymous';

}

function conversion(x,y){
		return (y*image.width+x)*4;	
	}

 
function traitement1(){
  var imageDataResult=ctxTraitement1.createImageData(image.width,image.height);
  var imageDataSource = ctxBase.getImageData(0, 0,image.width,image.height);

 
  var h=image.height;
  var w=image.width;

  // Boucles de parcours de toute l'image	
  for(var y=0;y<h;y++){
			for(var x=0;x<w;x++){
				posSrc=conversion(x,y);
				posDest=conversion(x,y);				
				gris[x+y*320]=(imageDataSource.data[posSrc+0]+imageDataSource.data[posSrc+1]+imageDataSource.data[posSrc+2])/3;
				imageDataResult.data[posDest+0]=gris[x+y*320];
				imageDataResult.data[posDest+1]=gris[x+y*320];
				imageDataResult.data[posDest+2]=gris[x+y*320];
				imageDataResult.data[posDest+3]=255;
			}
		}	
	ctxTraitement1.putImageData(imageDataResult,0,0); 
}

function traitement2(){
  var imageDataResult=ctxTraitement2.createImageData(image.width,image.height);

  //var trait=[[0.04,0.04,0.04,0.04,0.04],[0.04,0.04,0.04,0.04,0.04],[0.04,0.04,0.04,0.04,0.04],[0.04,0.04,0.04,0.04,0.04],[0.04,0.04,0.04,0.04,0.04]];
  var trait=[[0.01,0.03,0.03,0.03,0.01],[0.03,0.08,0.06,0.04,0.03],[0.03,0.08,0.09,0.08,0.03],[0.03,0.08,0.06,0.04,0.03],[0.01,0.03,0.03,0.03,0.01]];  
  //var trait=[[0,0,0,0,0],[0,0,0,0,0],[0,0,1,0,0],[0,0,0,0,0],[0,0,0,0,0]];
  
  var h=image.height-2;
  var w=image.width-2;

  // Boucles de parcours de toute l'image	
  for(var y=2;y<h;y++){
			for(var x=2;x<w;x++){
				var resultat=0;
				
				// Boucles de parcours du filtre	
				for(var l=-2;l<=2;l++){
					for(var c=-2;c<=2;c++){
						var coef=trait[c+2][l+2];
						resultat+=gris[x+c+(y+l)*320]*coef;
					}
				}
				gris2[x+y*320]=resultat;
				posDest=conversion(x,y);				
				imageDataResult.data[posDest+0]=resultat;
				imageDataResult.data[posDest+1]=resultat;
				imageDataResult.data[posDest+2]=resultat;
				imageDataResult.data[posDest+3]=255;
			}
	ctxTraitement2.putImageData(imageDataResult,0,0); 
	}

}
	
function traitement3(){
var imageDataResult=ctxTraitement3.createImageData(image.width,image.height);



  var h=image.height-3;
  var w=image.width-3;
  var trait=[-1,0,1];

  // Boucles de parcours de toute l'image	
  for(var y=3;y<h;y++){
			for(var x=3;x<w;x++){
				var resultatH=0;
					
				var resultatV=0;
				
				// Boucles de parcours du filtre	
				for(var l=-1;l<=1;l++){
						var coef=trait[l+1];						
						resultatV+=gris2[x+320*(y+l)]*coef;
	
				}		
				for(var c=-1;c<=1;c++){
						var coef=trait[c+1];
						resultatH+=gris2[x+c+320*y]*coef;

				}
				posDest=conversion(x,y);		
				var resultat=5*Math.sqrt(Math.pow(resultatV,2)+Math.pow(resultatH,2));
				contour[x+y*320]=resultat;
				imageDataResult.data[posDest+0]=resultat;
				imageDataResult.data[posDest+1]=resultat;
				imageDataResult.data[posDest+2]=resultat;
				imageDataResult.data[posDest+3]=255;
			}
		}	
		ctxTraitement3.putImageData(imageDataResult,0,0); 
}

function traitement4(){


	var h=image.height-3;
	var w=image.width-3;
	var seuilContour=100;
	var seuilHough=90;
	var roMax=400;
	var imageDataResult=ctxTraitement4.createImageData(nbRo,nbTeta);	
    // initialisation de la transformée de hough
	
	for (var teta=0;teta<nbTeta;teta++){
		for (var ro=0;ro<nbRo;ro++){
		  hough[nbRo*teta+ro]=0;
		}
	}
  
	// Boucles de parcours de la zone d'interet	
	for(var y=140;y<h;y++){
			for(var x=3;x<w;x++){
				if (contour[x+y*320]>seuilContour) {
					/*
					ctxTraitement3.strokeStyle = 'rgb(0,255,0)';
					ctxTraitement3.moveTo(x,y);
					ctxTraitement3.arc(x, y, 2, 0, 2 * Math.PI);
					*/
					// calcul de la transformée de hough
					for (var teta=0;teta<nbTeta;teta++) {
					  hough[Math.floor(nbRo*teta+nbRo/roMax*(x*Math.cos(teta/nbTeta*2*Math.PI)+y*Math.sin(teta/nbTeta*2*Math.PI)))]+=1;
					}
				}
			}
	}
	ctxTraitement3.stroke();
	
	// affichage de la transformée de hough
	for (var teta=0;teta<nbTeta;teta++){
	  for (var ro=0;ro<nbRo;ro++){			
				posDest=(teta+ro*nbTeta)*4;				

				imageDataResult.data[posDest+0]=hough[teta*nbRo+ro]*5;
				imageDataResult.data[posDest+1]=hough[teta*nbRo+ro]*5;
				imageDataResult.data[posDest+2]=hough[teta*nbRo+ro]*5;
				imageDataResult.data[posDest+3]=255;
				if (hough[teta*nbRo+ro]>seuilHough) {
					var xc=roMax*ro/nbRo*Math.cos(teta/nbTeta*2*Math.PI);
					var yc=roMax*ro/nbRo*Math.sin(teta/nbTeta*2*Math.PI);
					var dx=-roMax*Math.sin(teta/nbTeta*2*Math.PI);
					var dy=roMax*Math.cos(teta/nbTeta*2*Math.PI);
					
					ctxTraitement3.lineWidth='1';
					ctxTraitement3.strokeStyle = 'rgb(255,0,0)';
					ctxTraitement3.moveTo(xc-dx,yc-dy);
					ctxTraitement3.lineTo(xc+dx,yc+dy);
					//ctxTraitement3.moveTo(xc,yc);
					//ctxTraitement3.arc(xc, yc, 2, 0, 2 * Math.PI);
					
				}
			}
	}
	ctxTraitement3.stroke();	
	ctxTraitement4.putImageData(imageDataResult,0,0);

	
}
window.onload = cestparti;