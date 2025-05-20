using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CS_LIBRARY12
{
    public class MatrixOperations
    {
        public static void MultiplyMatrices(object parameter)
        {
            var parameters = (object[])parameter;
            int[,] matrix1 = (int[,])parameters[0];
            int[,] matrix2 = (int[,])parameters[1];
            int[,] resultMatrix = (int[,])parameters[2];
            int totalElements = (int)parameters[3];
            int processedElements = 0;

            int rows1 = matrix1.GetLength(0);
            int cols1 = matrix1.GetLength(1);
            int cols2 = matrix2.GetLength(1);

            // Mnożenie macierzy
            for (int i = 0; i < rows1; i++)
            {
                for (int j = 0; j < cols2; j++)
                {
                    resultMatrix[i, j] = 0;
                    for (int k = 0; k < cols1; k++)
                    {
                        resultMatrix[i, j] += matrix1[i, k] * matrix2[k, j];
                    }

                    processedElements++;
                    // Zgłaszamy postęp
                    ((CustomBackgroundWorker)parameters[4]).ReportProgress(processedElements);
                }
            }
        }
    }
}
