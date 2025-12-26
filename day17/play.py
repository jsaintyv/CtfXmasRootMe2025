import requests
import json
import os
import time
from PIL import Image
import numpy as np
import random
import csv

random.seed(10)



def testBlackDot(api, dx, dy, writer):
    # Ouvrir l'image en grayscale
    image_path = "base.png"
    image = Image.open(image_path).convert("L")  # "L" pour grayscale 8 bits
    arrayNice = np.array(image)


    # Ajouter un bruit aléatoire (entre -8 et +8)
    #  noise = np.random.randint(-3, 3, size=image_array.shape)
    # noisy_image_array = image_array + noise

    for y in range(32):
        for x in range(32):
            if dx == x and dy == y:
                if arrayNice[y][x] == 0:
                    arrayNice[y][x] = 0xff
                else:
                    arrayNice[y][x] = 0
    
    # Limiter les valeurs entre 0 et 255
    noisy_image_array = np.clip(arrayNice, 0, 255)

    # Convertir le tableau en image
    noisy_image = Image.fromarray(noisy_image_array.astype("uint8"))

    # Sauvegarder l'image
    noisy_image.save("test.png")

    predict = api.predict("test.png")
    print(f"{dx}, {dy} => {predict}")

    writer.writerow([dx, dy, predict["label"], predict["confidence"]])
    
    if predict["label"] != "Nice":
        writer
        return True     
    return predict["confidence"] < 0.531
    
        

def mergeAdv():
    # Ouvrir l'image en grayscale
    image_path = "base.png"
    image = Image.open(image_path).convert("L")  # "L" pour grayscale 8 bits
    arrayNice = np.array(image)

    image_path = "worst.png"
    imageNaughty = Image.open(image_path).convert("L")  # "L" pour grayscale 8 bits
    arrayNaughty = np.array(imageNaughty)

    # Ajouter un bruit aléatoire (entre -8 et +8)
    #  noise = np.random.randint(-3, 3, size=image_array.shape)
    # noisy_image_array = image_array + noise

    for y in range(32):
        for x in range(32):
            if arrayNice[y][x] == 0 and arrayNice[y][x] != arrayNaughty[y][x]:
                arrayNice[y][x] = 7
            else:
                if arrayNice[y][x] == 0xff and arrayNice[y][x] != arrayNaughty[y][x]:
                    arrayNice[y][x] = 0xff - 7
        

    # Limiter les valeurs entre 0 et 255
    noisy_image_array = np.clip(arrayNice, 0, 255)

    # Convertir le tableau en image
    noisy_image = Image.fromarray(noisy_image_array.astype("uint8"))

    # Sauvegarder l'image
    noisy_image.save("adv.png")

    print("Image avec bruit sauvegardée !")

def generateAdv():
    # Ouvrir l'image en grayscale
    image_path = "base.png"
    image = Image.open(image_path).convert("L")  # "L" pour grayscale 8 bits
    image_array = np.array(image)

    # Ajouter un bruit aléatoire (entre -8 et +8)
    #  noise = np.random.randint(-3, 3, size=image_array.shape)
    # noisy_image_array = image_array + noise

    for y in range(32):
        for x in range(32):

            if image_array[y][x] == 0:
                image_array[y][x] = random.randint(0, 7)
            else:
                if image_array[y][x] == 0xff:
                    image_array[y][x] = 0xff - random.randint(0, 7)
        

    # Limiter les valeurs entre 0 et 255
    noisy_image_array = np.clip(image_array, 0, 255)

    # Convertir le tableau en image
    noisy_image = Image.fromarray(noisy_image_array.astype("uint8"))

    # Sauvegarder l'image
    noisy_image.save("adv.png")

    print("Image avec bruit sauvegardée !")



def list_png_images(directory="."):
    """Liste les fichiers .png dans le dossier spécifié."""
    png_files = [f for f in os.listdir(directory) if f.lower().endswith('.png')]
    return png_files

class NaughtyOrNiceAPI:
    def __init__(self, base_url: str):
        self.base_url = base_url

    def health_check(self) -> dict:
        """Effectue un health check sur l'API."""
        response = requests.get(f"{self.base_url}/")
        return response.json()

    def get_info(self) -> dict:
        """Récupère les informations de l'API : epsilon, levels, width, height, classes, base_sha256."""
        response = requests.get(f"{self.base_url}/info")
        return response.json()

    def predict(self, image_path: str) -> dict:
        """
        Soumet une image pour prédiction.
        L'image doit être au format PNG, en niveaux de gris et de taille 32x32.
        """
        with open(image_path, 'rb') as image_file:
            files = {'image': (image_path, image_file, 'image/png')}
            response = requests.post(f"{self.base_url}/predict", files=files)
        return response.json()

    def submit(self, image_path: str) -> dict:
        """
        Soumet une image pour validation.
        L'image doit être au format PNG, en niveaux de gris et de taille 32x32.
        """
        with open(image_path, 'rb') as image_file:
            files = {'image': (image_path, image_file, 'image/png')}
            response = requests.post(f"{self.base_url}/submit", files=files)
        return response.json()

# Exemple d'utilisation
if __name__ == "__main__":
    api = NaughtyOrNiceAPI("http://dyn-02.xmas.root-me.org:11432")  # Remplacez par l'URL de votre API

    # Health check
    print("Health check:", api.health_check())

    # Récupérer les informations
    print("Info:", api.get_info())


    
    # Écrire les données dans le fichier CSV
    with open("prediction.csv", mode='w', newline='') as file:
        writer = csv.writer(file, delimiter=';')
        # Écrire l'en-tête
        writer.writerow(["x", "y", "label", "confidence"])
        # Écrire les lignes de données        

        naughty = np.zeros((32, 32), dtype=np.uint8)
        for y in range(32):
            for x in range(32):
                if testBlackDot(api, x, y, writer):
                    naughty[y][x] = 0
                else:
                    naughty[y][x] = 0xff
                time.sleep(0.75)

    # Limiter les valeurs entre 0 et 255
    arrayWorst = np.clip(naughty, 0, 255)

    # Convertir le tableau en image
    imageWorst = Image.fromarray(naughty.astype("uint8"))
    imageWorst.save("worst.png")


    mergeAdv()

    # Soumettre une image
    prediction = api.predict("adv.png")
    print("Submission:", prediction)

    # Soumettre une image
    submission = api.submit("adv.png")
    print("Submission:", submission)
