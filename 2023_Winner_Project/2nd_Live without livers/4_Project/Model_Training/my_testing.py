import numpy as np


class MyTester:
    def __init__(self,all_labels:list,all_predictions:list):
        """the input should be the information of original size of pictures,
        and each row of label should be in the format of [x1,y1,x2,y2,class,distance]
        and each row of prediction should be in the format of [x1, y1, x2, y2, conf, dist, cls]
        """
        self.consider_distance = True
        self.iou_threshold = 0.25 # the threshold of positive sample
        
        self.all_labels = all_labels
        self.all_predictions = all_predictions
        self.dist_error = 0
        
        self.tp = self.find_tp(self.consider_distance) #the number of true positive samples
        
        
    def find_tp(self,consider_distance = False):
        tp = 0
        repeated_tp = 0
        self.labels_len = 0
        self.predictions_len = 0
        self.total_dist_mse = 0
        
        

        for index,one_image_labels in enumerate(self.all_labels):
            
            self.labels_len+=one_image_labels.shape[0]
            
            one_image_predicions = self.all_predictions[index]
            if len(one_image_predicions) == 0:
                continue
            self.predictions_len += one_image_predicions.shape[0]
            # print(one_image_predicions.shape)
            for label in one_image_labels:
                one_label_tp = 0
                for prediction in one_image_predicions:
                    if prediction[6] == label[4] and (1 or not consider_distance or abs(prediction[5]-label[5])<0.2*np.clip(label[5],a_min=10)):
                        if self.bbox_iou(label[0:4],prediction[0:4]) > self.iou_threshold:
                            one_label_tp+=1
                            if consider_distance:
                                self.total_dist_mse+= abs(prediction[5]-label[5]) # *(prediction[5]-label[5])
                repeated_tp+=one_label_tp
                if one_label_tp >1:
                    print("find more than one corresponding predictions to one labels at image No.",index)
                if one_label_tp>0:
                    tp+=1
                # print(f'tp = {tp}, prediction len = {self.predictions_len}')
        self.dist_error = self.total_dist_mse/repeated_tp
        return tp
            
    def bbox_iou(self, box1, box2):
        # Returns the IoU of box1 to box2. box1 is 4, box2 is nx4
        eps=1e-7
        '''
        # Get the coordinates of bounding boxes
        b1_x1, b1_y1, b1_x2, b1_y2 = box1[0], box1[1], box1[2], box1[3]
        b2_x1, b2_y1, b2_x2, b2_y2 = box2[0], box2[1], box2[2], box2[3]

        # Intersection area
        inter = np.max((np.min(b1_x2, b2_x2) - np.max(b1_x1, b2_x1)),a_min=0   ) * \
                np.max((np.min(b1_y2, b2_y2) - np.max(b1_y1, b2_y1)),a_min=0   )
        # Union Area
        w1, h1 = b1_x2 - b1_x1, b1_y2 - b1_y1 + eps
        w2, h2 = b2_x2 - b2_x1, b2_y2 - b2_y1 + eps
        union = w1 * h1 + w2 * h2 - inter + eps
        '''
            # Calculate the overlap between a box and a list of boxes
        x1 = np.maximum(box1[0], box2[0])
        y1 = np.maximum(box1[1], box2[1])
        x2 = np.minimum(box1[2], box2[2])
        y2 = np.minimum(box1[3], box2[3])

        intersection = np.maximum(0, x2 - x1) * np.maximum(0, y2 - y1)
        area1 = (box1[2] - box1[0]) * (box1[3] - box1[1])
        area2 = (box2[2] - box2[0]) * (box2[3] - box2[1])
        union = area1 + area2 - intersection

        iou = intersection / union
        return iou  # IoU       
    
    def dist_mse(self):
        return self.dist_error
        
    def recall(self):
        """return the recall in floating point format"""
        return self.tp/self.labels_len
    def precision(self):
        """return the recall in floating point format"""
        return self.tp/self.predictions_len
    def accuracy(self):
        return self.tp/(self.labels_len+self.predictions_len-self.tp)
    
    def showResult(self):
        print(f'Recall = {self.recall()}')
        print(f'precision = {self.precision()}')
        print(f'accuracy = {self.accuracy()}')
        print(f'dist mse = {self.dist_mse()}')

        
        
