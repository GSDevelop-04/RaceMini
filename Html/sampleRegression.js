var btnTraitement1,
    btnTraitement2,
	ctx,
	image,
	base,
	canvasTraitement1,
	ctxTraitement1;

var nbTeta=180;
var nbRo=200;	

var gris = new Array(320*240);
var gris2 = new Array(320*240);
var contour = new Array(320*240);
var hough = new Array(nbRo*nbTeta);

function cestparti() {
	// on trouve l'image dans le html
	image = document.getElementById("image");
	image.crossOrigin = 'Anonymous';
	
	// on trouve le calque "Base" dans le html
	base = document.getElementById("canvasBase");
	// le contexte permettra d'y dessiner dessus
	ctxBase = base.getContext("2d");

	// on trouve le calque "traitement1" dans le html
	canvasTraitement1 = document.getElementById("canvasTraitement1");
	// et son contexte
	ctxTraitement1 = canvasTraitement1.getContext("2d");

	// on trouve les bouton "traitement1" et "traitement2"
	btnTraitement1=document.getElementById("traitement1");
	btnTraitement2=document.getElementById("traitement2");

	// et on leurs associe les routines correspondantes
	btnTraitement1.onclick=traitement1;
	btnTraitement2.onclick=traitement2;

	// on trace l'image dans le calque de base
	ctxBase.drawImage(image,0,0);
	ctxBase.crossOrigin = 'Anonymous';

}

function conversion(x,y){
		return (y*image.width+x)*4;	
	}

// traitement 1 converti en niveau de gris l'image
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

// dans la zone d'interet "ligne blache de droite" on trouve ligne par ligne la zone la plus claire puis calcule la droite de regression
function traitement2(){
  
  var h=image.height;
  var w=image.width;
  var max = new Array(100);
  var moyenne;
  
  // Boucles de parcours de toute l'image	
  // ligne par ligne
  for(var y=140;y<h;y++){
		niveau=0;
		// on parcourt toute la ligne pour trouver la zone la plus claire
		for(var x=2;x<w/2-2;x++){
			moyenne=(gris[x-2+320*y]+gris[x-1+320*y]+gris[x+320*y]+gris[x+1+320*y]+gris[x+2+320*y])/5; // on fait une moyenne sur 5 pixels
			if (moyenne>niveau) {
				niveau=moyenne;
				max[y-140]=x;
			}
		}
		ctxTraitement1.strokeStyle = 'rgb(0,255,0)';
		ctxTraitement1.moveTo(max[y-140],y);
		ctxTraitement1.arc(max[y-140], y, 2, 0, 2 * Math.PI);
	}
	ctxTraitement1.stroke();
	
	
	// regression
	var nivMoy=0;
	var yMoy=49.5;
	
	//calcul nivMoy
	var nivMoy=0;	
	for (var y=0;y<100;y++){
		nivMoy+=max[y];
	}
	nivMoy=nivMoy/100;
	
	//calcul variance
	var variance=0;
	for (var y=0;y<100;y++){
		variance+=y*y;
	}
	variance=variance/100-yMoy*yMoy;
	
	//calcul covariance
	var coVariance=0;
	for (var y=0;y<100;y++){
		coVariance+=max[y]*y;
	}
	coVariance=coVariance/100-nivMoy*yMoy;
	var a=coVariance/variance;
	var b=nivMoy-a*yMoy;
	console.log(a);
	console.log(b);
	
	// affichage de la droite sur l'image de base
						
	ctxBase.lineWidth='4';
	ctxBase.strokeStyle = 'rgb(255,0,0)';
	ctxBase.moveTo(b,140);
	ctxBase.lineTo(a*100+b,240);
	ctxBase.stroke();
}




window.onload = cestparti;