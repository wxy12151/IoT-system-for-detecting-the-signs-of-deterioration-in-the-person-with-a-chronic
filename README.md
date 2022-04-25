# ELEC0130_21-22_Internet-of-Things-

```
├─Phase I
│  ├─Arduino
│  │  ├─AbnormalSound_and_NightWandering
│  │  └─MemoryLoss
│  └─NodeRED
└─Phase II
    ├─classification_report
    ├─Datasets
    ├─EDA_code
    │  └─Matlab_heatmaps
    │      └─Matlab_heatmaps
    ├─heatmap on floorplan
    ├─images
    │  ├─images_AbnormalIdentification
    │  │  ├─feature_importance
    │  │  └─KMeans_results
    │  ├─images_ActivityRecognition
    │  │  └─importance_per_activity
    │  ├─images_EDA
    │  │  ├─correlations
    │  │  ├─freq_span
    │  │  │  └─z-score
    │  │  ├─heatmap_floorplan
    │  │  └─time_series_patterns
    │  └─images_LSTM
    └─models
        └─LSTMWithoutAux
            └─variables
```

# 1. Brief introduction

As the population ages, a rising number of elderly are plagued by a variety of age-related disorders. For example, dementia is one of the most prevalent diseases among the elderly, affecting more than 50 million people globally and anticipated to quadruple in prevalence by 2050. In such a situation, it is not only expensive for medical care-delivery systems, but also cumbersome and time-consuming for patients to receive treatment in the conventional manner. 

Thus, it is critical to develop a variety of automated systems that can assist in recognizing the onset or progression of related ageing diseases such as dementia, reminding caregivers when appropriate, performing long-term observation and statistics, and so on. Fortunately, with the rapid growth of Internet of Things (IoT) and Machine Learning (ML) techniques in recent years, these system designs are becoming a reality and assisting in the saving of massive amounts of human resources.

# 2. Organization of the files

## 2.1. Phase I

**Arduino:**

- ![avatar](/README_images/sensor_selection.png)



## 2.2. Phase II

This folder divides the entire project process into data preprocessing, model, train, and test procedures, waiting for calling by "main.py".

**All functions within this folder with detailed comments**

### 2.2.1. File "data_preprocessing.py"

1. Split into the train(validation within it) and test

2. Extract useful features and labels

3. Set your model output as categorical and save it in the new label column

4. Split the training set into training and validation dataset

5. tokenizer function (in "train.py")

   Return the training, validation and testing datasets.

### 2.2.2. File "model.py"

Define model structures of BERT, RoBERTa, DistilBERT and XLNet.

### 2.2.3. File "train.py"

Define the training pipelines with an optimizer, loss, and other hyper-parameters adjustable.

Return the model after training and logs history to analyze training convergence.

### 2.2.4. File "test.py"

This part evaluates the model performance on the testing dataset containing:

1. Classification Reports.

2. Confusion Matrix figures.

3. Multi-class ROC figures.

4. Globally micro ROC figures.

## 2.3. Other Folders

Folder  "images" save the training and testing figures, as well as model structures.

Folder  "model_logs" saves the training logs history.

Folder  "model_trained" saves the model after training.

Folder  "submission" saves the .csv file using the same format as the competition.

# 3. Run the code

## 3.1. Run the code on "main.py"

Notes. It will run all steps of this project by calling the functions in the folder "function".

## 3.2. Run the code on "main.ipynb"

Notes. This contains more detailed work than in "main.py", including the EDA process at the beginning, open in Jupyter for running step by step.

### All trained models employed in the report can be obtained in the  [GOOGLE DRIVE](https://drive.google.com/drive/folders/1STGwHfYwuoKsOVNKbPQgW3_PtNOjfgqk?usp=sharing) link

### DO NOT CHANGE THE FILE LEVEL



























