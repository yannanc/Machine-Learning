# -*- coding: utf-8 -*-
"""
Andrew ID: yannanc

This file uses PyTorch to complete the Google Landmark Recognition Challenge, 
which trains a model to classify 10 locations.
After loading the train data, this file loads a pretrained ResNet18 model, 
reset final fully connected layer(output_size=10, lr=0.001, momentum=0.9) 
and train the model with 2 epoches.
Finally, put the neural network’s prediction result on test dataset into submission.txt.

"""

from __future__ import print_function, division
import os
import torch
import torch.nn as nn
import torch.optim as optim
from torch.optim import lr_scheduler
from torch.autograd import Variable
import pandas as pd
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
from torch.utils.data import Dataset, DataLoader
import torchvision
from torchvision import transforms

plt.ion()   # interactive mode

class GoogleLandmarksDataset(Dataset):
    """Google Landmarks train dataset."""

    def __init__(self, csv_file, root_dir, transform=None):
        """
        Args:
            csv_file (string): Path to the csv file with annotations.
            root_dir (string): Directory with all the images.
            transform (callable, optional): Optional transform to be applied
                on a sample.
        """
        self.landmarks_frame = pd.read_csv(csv_file)
        self.root_dir = root_dir
        self.transform = transform

    def __len__(self):
        return len(self.landmarks_frame)

    def __getitem__(self, idx):
        img_name = os.path.join(self.root_dir,
                                self.landmarks_frame.iloc[idx, 1]+'.jpg')
        image = Image.open(img_name)
        label = self.landmarks_frame.iloc[idx, 3].astype(np.int64)

        if self.transform:
            image = self.transform(image)

        return image, label
    
class testDataset(Dataset):
    """Google Landmarks test dataset."""

    def __init__(self, csv_file, root_dir, transform=None):
        """
        Args:
            csv_file (string): Path to the csv file with annotations.
            root_dir (string): Directory with all the images.
            transform (callable, optional): Optional transform to be applied
                on a sample.
        """
        self.landmarks_frame = pd.read_csv(csv_file)
        self.root_dir = root_dir
        self.transform = transform
    
    def __len__(self):
        return len(self.landmarks_frame)

    def __getitem__(self, idx):
        img_name = os.path.join(self.root_dir,
                                self.landmarks_frame.iloc[idx, 1]+'.jpg')
        image = Image.open(img_name)
        
        if self.transform:
            image = self.transform(image)

        return image

def train_model(model, criterion, optimizer, scheduler, num_epochs=2):

    for epoch in range(num_epochs):
        print('Epoch {}/{}'.format(epoch, num_epochs - 1))
        print('-' * 10)

        scheduler.step()
        model.train()  # Set model to training mode
        
        running_loss = 0.0

        # Iterate over data.
        for i_batch, (image, label) in enumerate(dataloader):
            #for image, label in dataloader:
            inputs = Variable(image)
            labels = Variable(label)
            # zero the parameter gradients
            optimizer.zero_grad()
            # forward
            outputs = model(inputs)
            loss = criterion(outputs, labels)
            # backward + optimize
            loss.backward()
            optimizer.step()
            # statistics
            running_loss += loss.item() * inputs.size(0)
            if i_batch%10 == 0:
                print(i_batch, loss.item(), running_loss)

        epoch_loss = running_loss / dataset_sizes
        print('Loss: {:.4f} '.format(epoch_loss))
        print()

    return model


# input train data
transformed_dataset = GoogleLandmarksDataset(csv_file='hw7data/train.csv',
                                           root_dir='hw7data/images/',
                                           transform=transforms.Compose([
                                               transforms.RandomResizedCrop(224),
                                               transforms.ToTensor(),
                                               transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
                                           ]))
dataloader = DataLoader(transformed_dataset, batch_size=64, shuffle=True, num_workers=0)
dataset_sizes = len(transformed_dataset)


# input test data
transformed_testdataset = testDataset(csv_file='hw7data/test.csv',
                                           root_dir='hw7data/images/',
                                           transform=transforms.Compose([
                                               transforms.Resize(224),
                                               transforms.ToTensor(),
                                               transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
                                           ]))
testdataloader = DataLoader(transformed_testdataset, batch_size=32, shuffle=False, num_workers=0)

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")


# train model
model_conv = torchvision.models.resnet18(pretrained=True)
    
# Parameters of newly constructed modules have requires_grad=True by default
num_ftrs = model_conv.fc.in_features
model_conv.fc = nn.Linear(num_ftrs, 10)
model_conv = model_conv.to(device)
criterion = nn.CrossEntropyLoss()
# Observe that only parameters of final layer are being optimized as opoosed to before.
optimizer_conv = optim.SGD(model_conv.parameters(), lr=0.001, momentum=0.9, weight_decay=0.1)
# Decay LR by a factor of 0.1 every 7 epochs
exp_lr_scheduler = lr_scheduler.StepLR(optimizer_conv, step_size=7, gamma=0.1)

model_conv = train_model(model_conv, criterion, optimizer_conv,
                         exp_lr_scheduler, num_epochs=2)

torch.save(model_conv, 'model.pkl') # save the model


# output test result
f = open("submission.txt", "w")
model_conv.eval()
print("landmark_id", file = f)
for i_batch, image in enumerate(testdataloader):
        #for inputs, labels in dataloader:
        inputs = Variable(image)
        outputs = model_conv(inputs)
        _, preds = torch.max(outputs, 1)
        size = torch.numel(preds)
        for i in range(0, size):
            print(preds[i].item(), file = f)
        print(i_batch)
f.close()